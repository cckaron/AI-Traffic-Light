#define RED1 22
#define GREEN1 23
#define RED2 26
#define GREEN2 27
#define RED3 24
#define GREEN3 25
#define RED4 28
#define GREEN4 29
 
String DIR1 = "49"; //ASCII -> 1
String DIR2 = "50"; //ASCII -> 2

String received;

void setup() {
  initSerial();

  initLight();

  Serial1.write(100); //transmit init message to main controller
}

void loop() {
  //listen to main controller
  listenToMainController();
}

void initSerial() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void initLight() {
  pinMode(RED1, OUTPUT);
  pinMode(GREEN1, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(RED3, OUTPUT);
  pinMode(GREEN3, OUTPUT);
  pinMode(RED4, OUTPUT);
  pinMode(GREEN4, OUTPUT);

  digitalWrite(RED1, HIGH);
  digitalWrite(GREEN1, LOW);
  digitalWrite(RED2, HIGH);
  digitalWrite(GREEN2, LOW);
  digitalWrite(RED3, LOW);
  digitalWrite(GREEN3, HIGH);
  digitalWrite(RED4, LOW);
  digitalWrite(GREEN4, HIGH);
}

void listenToMainController() {
  if (Serial1.available() > 0) {
    received = Serial1.read();
    Serial.println(received);

    if (received == DIR1) {
      digitalWrite(RED1, HIGH);
      digitalWrite(GREEN1, LOW);
      digitalWrite(RED2, HIGH);
      digitalWrite(GREEN2, LOW);
      digitalWrite(RED3, LOW);
      digitalWrite(GREEN3, HIGH);
      digitalWrite(RED4, LOW);
      digitalWrite(GREEN4, HIGH);

    } else if (received == DIR2) {
      digitalWrite(RED1, LOW);
      digitalWrite(GREEN1, HIGH);
      digitalWrite(RED2, LOW);
      digitalWrite(GREEN2, HIGH);
      digitalWrite(RED3, HIGH);
      digitalWrite(GREEN3, LOW);
      digitalWrite(RED4, HIGH);
      digitalWrite(GREEN4, LOW);
    }
  }
}
