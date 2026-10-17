
#include <Arduino.h>
#include <math.h>

// =====================================================
// ESP32-S3 Ultrasonic Anemometer
// =====================================================

// Transducer separation in metres
constexpr float TRANSDUCER_DISTANCE = 0.150f;

// PWM configuration
constexpr uint32_t PWM_FREQUENCY = 40000;
constexpr uint8_t PWM_RESOLUTION = 8;
constexpr uint32_t PULSE_DURATION_US = 400;

// Measurement timing
constexpr uint32_t MEASUREMENT_TIMEOUT_US = 5000;
constexpr uint32_t RECEIVER_SETTLE_TIMEOUT_US = 2000;
constexpr uint32_t SAMPLE_PERIOD_MS = 100;

// GPIO assignments from the original program
constexpr int TX_MUX_S0 = 13;
constexpr int TX_MUX_S1 = 1;
constexpr int RX_MUX_S0 = 12;
constexpr int RX_MUX_S1 = 2;

constexpr int DRIVER_ENABLE = 5;
constexpr int RX_SIGNAL = 4;

// Direction enumeration
enum Direction {
  NORTH = 0,
  EAST  = 1,
  SOUTH = 2,
  WEST  = 3
};

// Transit-time measurement
struct Measurement {
  float transitTimeUs;
  bool valid;
};

Measurement NS = {0.0f, false};
Measurement SN = {0.0f, false};
Measurement EW = {0.0f, false};
Measurement WE = {0.0f, false};

// Calculated results
float windSpeed = 0.0f;
float windDirection = 0.0f;

uint32_t lastMeasurement = 0;
bool windResultValid = false;

// Function declarations
void initializeHardware();
void selectTransmit(Direction dir);
void selectReceive(Direction dir);
void generateBurst();
float measureTransit(Direction tx, Direction rx);
bool calculateWind();
void printTelemetry();

// =====================================================
// SETUP — defined only once
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  initializeHardware();

  Serial.println();
  Serial.println("================================");
  Serial.println("ULTRASONIC ANEMOMETER");
  Serial.println("ESP32-S3 INITIALIZED");
  Serial.println("================================");
}

// =====================================================
// LOOP — defined only once
// =====================================================
void loop() {
  if ((uint32_t)(millis() - lastMeasurement) >=
      SAMPLE_PERIOD_MS) {

    lastMeasurement = millis();

    NS.transitTimeUs = measureTransit(NORTH, SOUTH);
    NS.valid = NS.transitTimeUs > 0.0f;

    SN.transitTimeUs = measureTransit(SOUTH, NORTH);
    SN.valid = SN.transitTimeUs > 0.0f;

    EW.transitTimeUs = measureTransit(EAST, WEST);
    EW.valid = EW.transitTimeUs > 0.0f;

    WE.transitTimeUs = measureTransit(WEST, EAST);
    WE.valid = WE.transitTimeUs > 0.0f;

    windResultValid = calculateWind();
    printTelemetry();
  }
}

// =====================================================
// HARDWARE INITIALIZATION
// =====================================================
void initializeHardware() {
  pinMode(TX_MUX_S0, OUTPUT);
  pinMode(TX_MUX_S1, OUTPUT);
  pinMode(RX_MUX_S0, OUTPUT);
  pinMode(RX_MUX_S1, OUTPUT);

  pinMode(DRIVER_ENABLE, OUTPUT);
  digitalWrite(DRIVER_ENABLE, LOW);

  pinMode(RX_SIGNAL, INPUT);

  // ESP32 Arduino core 3.x LEDC API
  if (!ledcAttach(
        DRIVER_ENABLE,
        PWM_FREQUENCY,
        PWM_RESOLUTION)) {

    Serial.println("ERROR: PWM initialization failed.");

    while (true) {
      delay(1000);
    }
  }

  ledcWrite(DRIVER_ENABLE, 0);

  Serial.println("PWM configured for 40 kHz.");
}

// =====================================================
// TRANSMIT MULTIPLEXER SELECTION
// =====================================================
void selectTransmit(Direction dir) {
  digitalWrite(TX_MUX_S0,
               (dir & 0x01) ? HIGH : LOW);

  digitalWrite(TX_MUX_S1,
               (dir & 0x02) ? HIGH : LOW);
}

// =====================================================
// RECEIVE MULTIPLEXER SELECTION
// =====================================================
void selectReceive(Direction dir) {
  digitalWrite(RX_MUX_S0,
               (dir & 0x01) ? HIGH : LOW);

  digitalWrite(RX_MUX_S1,
               (dir & 0x02) ? HIGH : LOW);
}

// =====================================================
// GENERATE ULTRASONIC BURST
// =====================================================
void generateBurst() {
  // 8-bit resolution: 128 is approximately 50% duty
  ledcWrite(DRIVER_ENABLE, 128);

  delayMicroseconds(PULSE_DURATION_US);

  ledcWrite(DRIVER_ENABLE, 0);
}

// =====================================================
// MEASURE TRANSIT TIME
// =====================================================
float measureTransit(Direction tx, Direction rx) {
  selectTransmit(tx);
  selectReceive(rx);

  // Allow switching circuitry to settle
  delayMicroseconds(20);

  // Wait for the receiver output to return LOW
  uint32_t waitStart = micros();

  while (digitalRead(RX_SIGNAL) == HIGH) {
    if ((uint32_t)(micros() - waitStart) >=
        RECEIVER_SETTLE_TIMEOUT_US) {
      return -1.0f;
    }
  }

  // Start timing before transmitting
  uint32_t startTime = micros();

  generateBurst();

  // Wait for an active-HIGH receiver detection signal
  while (digitalRead(RX_SIGNAL) == LOW) {
    if ((uint32_t)(micros() - startTime) >=
        MEASUREMENT_TIMEOUT_US) {
      return -1.0f;
    }
  }

  uint32_t stopTime = micros();

  return (float)(uint32_t)(stopTime - startTime);
}

// =====================================================
// CALCULATE WIND SPEED AND DIRECTION
// =====================================================
bool calculateWind() {
  if (!NS.valid || !SN.valid ||
      !EW.valid || !WE.valid) {
    return false;
  }

  float tNS = NS.transitTimeUs * 1.0e-6f;
  float tSN = SN.transitTimeUs * 1.0e-6f;
  float tEW = EW.transitTimeUs * 1.0e-6f;
  float tWE = WE.transitTimeUs * 1.0e-6f;

  if (tNS <= 0.0f || tSN <= 0.0f ||
      tEW <= 0.0f || tWE <= 0.0f) {
    return false;
  }

  // East-west wind component
  float vx = (TRANSDUCER_DISTANCE / 2.0f) *
             ((1.0f / tEW) - (1.0f / tWE));

  // North-south wind component
  float vy = (TRANSDUCER_DISTANCE / 2.0f) *
             ((1.0f / tNS) - (1.0f / tSN));

  windSpeed = sqrtf(vx * vx + vy * vy);

  // Direction measured clockwise from North
  windDirection = atan2f(vx, vy) * 180.0f / PI;

  if (windDirection < 0.0f) {
    windDirection += 360.0f;
  }

  return true;
}

// =====================================================
// SERIAL TELEMETRY
// =====================================================
void printTelemetry() {
  Serial.println("-------------------------------");

  Serial.print("N -> S: ");
  Serial.print(NS.transitTimeUs, 2);
  Serial.println(" us");

  Serial.print("S -> N: ");
  Serial.print(SN.transitTimeUs, 2);
  Serial.println(" us");

  Serial.print("E -> W: ");
  Serial.print(EW.transitTimeUs, 2);
  Serial.println(" us");

  Serial.print("W -> E: ");
  Serial.print(WE.transitTimeUs, 2);
  Serial.println(" us");

  if (!windResultValid) {
    Serial.println(
      "Wind result invalid: check receiver signal."
    );
    return;
  }

  Serial.print("Wind Speed: ");
  Serial.print(windSpeed, 3);
  Serial.println(" m/s");

  Serial.print("Wind Direction: ");
  Serial.print(windDirection, 2);
  Serial.println(" degrees");
}
