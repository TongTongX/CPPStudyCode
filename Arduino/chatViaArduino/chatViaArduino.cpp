/*
	Alan(Xutong) Zhao
	Yue Ma

	Acknowledgement: I worked with and received help from Yu Zhu.
	Acknowledgement: I received help from our TA (Xiaowei Hu) during the office hour on Wednesday.
	Acknowledgement: I used some funcitons that our professor(Csaba Szepesvari)developed in class, as well as functions on eClass.

*/


#include <Arduino.h>

// Reads a number from the keyboard by using the Serial Port
uint32_t get_number();
// returns the value of a ** b
uint32_t pow(uint32_t base, uint32_t power);
// take the mulplication and then modulus
uint32_t mul_mod(uint32_t a,uint32_t b ,uint32_t m);
// g**a % p, to calculate our public key
uint32_t pow_mod(uint32_t base, uint32_t power, uint32_t m);
// creates 32-bits random number
uint32_t get_random(int analogPin);
// test pow_mod
void test_pow_mod(uint32_t base, uint32_t power, uint32_t m, uint32_t exp_result);
void test_pow_mod();
// test get_random
void test_get_random(int analogPin);
// set up the shared_key
uint32_t key_set_up();
// conversition begins
void conversation(uint32_t shared_key);
// server
uint32_t server(uint32_t skey);
// client
uint32_t client(uint32_t ckey);
// read from serial3
uint32_t uint32_from_serial3();
uint32_t read_from_serial3();
// write to serial3
void uint32_to_serial3(uint32_t num);
void write_to_serial3(uint32_t num);
// wait
bool wait_on_serial3( uint8_t nbytes, long timeout ) {
  unsigned long deadline = millis() + timeout;
  while (Serial3.available()<nbytes && (timeout<0 || millis()<deadline))
  {
    delay(1);
  }
  return Serial3.available()>=nbytes;
}
uint32_t next_key(uint32_t current_key);

int main() {
  // Initialise Arduino functionality
  init();
  // Attach USB for applicable processors
  #ifdef USBCON
     USBDevice.attach();
  #endif
  Serial.begin(9600);      // Serial0: communication with the PC
  Serial3.begin(9600);     // Serial3: communication with the other Arduino
  uint32_t shared_key = key_set_up();
  conversation(shared_key);
  Serial.end();
  Serial3.end();
  return 0;
}
// client
uint32_t client(uint32_t ckey) {
  typedef enum { start=0, waiting_for_ACK, data_exchange} State;
  char* stateNames[] = {"start","waiting_for_ACK","data_exchange"};
  State state = start;
  uint32_t skey;
  while (state != data_exchange) {
    if  (state == start) {
      Serial3.write('C');
      uint32_to_serial3(ckey);
      state = waiting_for_ACK;
    }
    else if (state == waiting_for_ACK && wait_on_serial3(5,1000)) {
      // get acknowledgement
      char ACK = Serial3.read();
      if (ACK == 'A') {
        // save the value of the Server's key
        skey = uint32_from_serial3();
        // send acknowledgement        
        Serial3.write('A');
        // Data Exchange start
        state = data_exchange;
      }
    }
    else { state = start;}
    Serial.print("State:"); Serial.println(stateNames[state]);
  }
  return skey;
}
// server
uint32_t server(uint32_t skey) {
  typedef enum { listen=0, wait_for_key_1, wait_for_ACK_1, wait_for_key_2, wait_for_ACK_2, data_exchange} State;
  char* stateNames[] = { "listen", "wait_for_key_1", "wait_for_ACK_1", "wait_for_key_2", "wait_for_ACK_2", "data_exchange"};
  State state = listen;
  uint32_t ckey;
  char ACK;
  while (state != data_exchange) {
    if (state == listen) {
      char CR = Serial3.read();
      if (CR == 'C'){
        //Serial.print(CR);
        state = wait_for_key_1;
      }
    }
    else if (state == wait_for_key_1 && wait_on_serial3(4,1000)){
      ckey = uint32_from_serial3();
      Serial.println(ckey);
      Serial3.write('A');
      uint32_to_serial3(skey);
      state = wait_for_ACK_1;
    }
    else if (state == wait_for_ACK_1 && wait_on_serial3(1,1000)){
      ACK = Serial3.read();
      if (ACK == 'A'){
        state = data_exchange;
      }
      if (ACK == 'C'){
        state = wait_for_key_2;
      }
    }
    else if (state == wait_for_key_2 && wait_on_serial3(4,1000)){
      ckey = uint32_from_serial3();
      state = wait_for_ACK_2;
    }
    else if (state == wait_for_ACK_2 && wait_on_serial3(1,1000)){
      ACK = Serial3.read();
      if (ACK == 'A'){
        state = data_exchange;
      }
      else{
        state = wait_for_key_2;
      }
    }
    else{
      state == listen;
    }
    Serial.print("State:"); Serial.println(stateNames[state]);
  }
  return ckey;
}
// read from serial3
uint32_t uint32_from_serial3() {
  uint32_t num = 0;
  num = num | ((uint32_t) Serial3.read()) << 0;
  num = num | ((uint32_t) Serial3.read()) << 8;
  num = num | ((uint32_t) Serial3.read()) << 16;
  num = num | ((uint32_t) Serial3.read()) << 24;
  return num;
}
// write to serial3
void uint32_to_serial3(uint32_t num) {
  Serial3.write((char) (num >> 0));
  Serial3.write((char) (num >> 8));
  Serial3.write((char) (num >> 16));
  Serial3.write((char) (num >> 24));
}
// set up the shared key
uint32_t key_set_up(){
  uint32_t prime = 2147483647;       // prime
  uint32_t generator = 16807;        // generator in Diffie-Hellman
  int analogPin = 1;                 // setup AnalogPin 
  uint32_t private_key = get_random(analogPin);    // create random private key
  uint32_t our_public_key = pow_mod(generator,private_key,prime);     // generator**private_key % prime
  Serial.print("Our public key is "); Serial.println(our_public_key); // display our public key
  uint32_t their_public_key;         // declare their_public_key
  const int inPin = 13;
  pinMode(inPin, INPUT);
  // check if I'm the client or server
  if (digitalRead(13) == 1) {
    // server
    their_public_key = server(our_public_key);
  } else {
    // client
    their_public_key = client(our_public_key);
  }
  Serial.print("their_public_key"); Serial.println(their_public_key); 
  uint32_t shared_key = pow_mod(their_public_key,private_key,prime);  // their_public_key**private_key % prime
  Serial.print("The shared key is "); Serial.println(shared_key);     // display the shared key
  return shared_key;
}
// Implements the Park-Miller algorithm with 32 bit integer arithmetic
uint32_t next_key(uint32_t current_key) {
  const uint32_t modulus = 0x7FFFFFFF; // 2^31-1
  const uint32_t consta = 48271; // we use that this is <=15 bits
  uint32_t lo = consta*(current_key & 0xFFFF); 
  uint32_t hi = consta*(current_key >> 16);
  lo += (hi & 0x7FFF)<<16;
  lo += hi>>15;
  if (lo > modulus) lo -= modulus;
  return lo;
}
// conversation begins
void conversation(uint32_t shared_key){
  // Loop procedure
  while (true) {
    // read from Serial3, write on Serial
    // (other Arduino to PC)
    char firstByte;
    if (Serial3.available()) {
      firstByte = Serial3.read();
      firstByte = firstByte ^ (shared_key & 0xFF);   // decryption 
      Serial.write( firstByte );
      shared_key = next_key(shared_key);
    }
    // read from Serial, write on Serial3
    // (PC to other Arduino)
    char secondByte;
    if (Serial.available()) {
      secondByte = Serial.read();
    
      /*Serial.println();
      Serial.print("The original key 2 is: ");
      Serial.println(shared_key);*/
    
      Serial.write( secondByte );
      secondByte = secondByte ^ (shared_key & 0xFF); // encryption
      Serial3.write( secondByte );
      shared_key = next_key(shared_key);
    
      /*Serial.println();
      Serial.print("The changed key 2 is: ");
      Serial.println(shared_key);*/
    }
  }
}
// returns the value of a ^ b
uint32_t pow(uint32_t base, uint32_t power){
  uint32_t binary = power;
  int i;
  uint32_t ret = 1;
  uint32_t new_base = base;
  for (i = 0; i <= 31; ++i){
    if (binary & (1 << i)){
      ret = ret * new_base;
    }
    new_base = (new_base * new_base);
  }
  return ret;
}
// a * b % m
uint32_t mul_mod( uint32_t a, uint32_t b, uint32_t m ) {
  const int bits = 32;
  uint32_t result = 0;
  uint32_t x = b % m;
  uint32_t y = a % m;
  uint32_t indicator = 1;
  for (int i = 0; i < bits; i++) {
    if ((y & (indicator << i)) != 0) {
      result = result + x % m;
      result = result % m;
    }
    x = x << 1;
    x = x % m;
  }
  return result; 
}
// g**a % p, to calculate our public key
uint32_t pow_mod(uint32_t base, uint32_t power, uint32_t m){
  uint32_t binary = power;
  int i;
  uint32_t ret = 1;
  uint32_t new_base = base % m;
  uint32_t indicator = 1;
  for (i = 0; i <= 31; ++i){
    if (binary & (indicator << i)){
      ret = mul_mod(ret, new_base, m);
    }
    new_base = mul_mod(new_base, new_base, m);
  }
  return ret % m;
}
// creates 32-bits random number
uint32_t get_random(int analogPin){
  uint32_t i;
  uint32_t random_array[32];
  uint32_t random = 0;
  uint32_t indicator = 1;
  for (i = 0; i <= 31; i++){
    random_array[i] = analogRead(analogPin) & indicator;
    random += random_array[i]*pow(2,i);
    delay(50);
  }
  return random;
}
// test pow mod
void test_pow_mod(uint32_t base, uint32_t power, uint32_t m, uint32_t exp_result){
  uint32_t result;
  result = pow_mod(base, power, m);
  if (result != exp_result){
    Serial.println("FAIL!");
    Serial.print("base: ");
    Serial.println(base);
    Serial.print("power: ");
    Serial.println(power);
    Serial.print("modulus: ");
    Serial.println(m);
    Serial.print("calculated result: ");
    Serial.println(result);
    Serial.print("expected result: ");
    Serial.println(exp_result);     
  }
}
void test_pow_mod(){
  Serial.println("Test pow_mod start");
  test_pow_mod(16807,1234567398,2147483647,1958014611);
  test_pow_mod(124456,1223247398,214343647,24359561);
  test_pow_mod(16807,1267398,21474847,18881667); 
  Serial.println("Test pow_mod finished");
}
// test get_random
void test_get_random(int analogPin){
  Serial.println(get_random(analogPin));
  Serial.println(get_random(analogPin));
  Serial.println(get_random(analogPin));
  Serial.println(get_random(analogPin));
  Serial.println(get_random(analogPin));
  Serial.println(get_random(analogPin));
  Serial.println(get_random(analogPin));     
} 
