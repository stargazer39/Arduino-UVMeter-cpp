void setup() {
  Serial.begin(115200);
  
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT);
}
const int BUFFER_SIZE = 200;
byte buf[BUFFER_SIZE];

void loop() {
//  for(int i = 0; i < BUFFER_SIZE; i++){
//    buf[i] = 0; 
//  }
  if(Serial.available() > 0){
    Serial.readBytes(buf, BUFFER_SIZE);
    analogWrite(9, buf[0]);
    analogWrite(10, buf[1]);
    analogWrite(11, buf[2]);
    analogWrite(3, buf[3]);
  }
}