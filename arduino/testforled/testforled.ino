#define RED1 22
#define GREEN1 23
#define RED2 24
#define GREEN2 25
#define RED3 26
#define GREEN3 27
#define RED4 28
#define GREEN4 29 //28 29 供電有問題!!

int received;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(RED1, OUTPUT);
  pinMode(GREEN1, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  pinMode(RED3, OUTPUT);
  pinMode(GREEN3, OUTPUT);
  pinMode(RED4, OUTPUT);
  pinMode(GREEN4, OUTPUT);
  
  digitalWrite(RED1, LOW);
  digitalWrite(GREEN1, LOW);
  digitalWrite(RED2, LOW);
  digitalWrite(GREEN2, LOW);
  digitalWrite(RED3, LOW);
  digitalWrite(GREEN3, LOW);
  digitalWrite(RED4, HIGH);
  digitalWrite(GREEN4, HIGH);

  //  Serial1.write(100); //init message
}

void loop() {

}
