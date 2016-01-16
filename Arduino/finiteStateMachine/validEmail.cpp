/*
 * Verifying email addresses
 */

#include <Arduino.h>




bool isValidEMAIL(char* str){
  typedef enum {first, rest, 
				server_id1_first, server_id1_rest, 
				server_id2_first, server_id2_rest,
				server_id3_first, server_id3_rest,
				server_id4_first, server_id4_rest, 
				server_id_end, ERR, OK} State;
  char* stateNames[] = {"client_id_first", "client_id_rest", 
						"server_id1_first", "server_id1_rest", 
						"server_id2_first", "server_id2_rest",
						"server_id3_first", "server_id3_rest",
						"server_id4_first", "server_id4_rest",
						"server_id_end", "ERR", "OK"};
  State state = first;
  int i = 0;
  while((state!= OK) && (state!=ERR)){
    char c = str[i];
      
    // state == first 
    if      ((state == first) && (isalpha(c) || c == '_'))	{ state = rest; }
    else if ((state == first) && (c ==' '))					{ ; }    
    // state == rest
    else if ((state == rest) && c == '@')					{ state = server_id1_first; }
    else if ((state == rest) && (isalpha(c) || isdigit(c) || c=='-' || c=='_' || c=='.'))	{ ; }
    // state == server_id1_first
    else if ((state == server_id1_first) && isalpha(c))		{ state = server_id1_rest; }
	// state == server_id1_rest
    else if ((state == server_id1_rest) && isalpha(c))		{ ; } 
    else if ((state == server_id1_rest) && c=='.')			{ state = server_id2_first; }
    // state == server_id2_first
    else if ((state == server_id2_first) && isalpha(c))		{ state = server_id2_rest; }
    // state == server_id2_rest
    else if ((state == server_id2_rest) && isalpha(c))		{ ; } 
    else if ((state == server_id2_rest) && c=='.')			{ state = server_id3_first; }
    else if ((state == server_id2_rest) && c==' ')			{ state == server_id_end; }
    else if ((state == server_id2_rest) && c==0)			{ state = OK; }
    // state == server_id3_first
    else if ((state == server_id3_first) && isalpha(c))		{ state = server_id3_rest; }
    // state == server_id3_rest
    else if ((state == server_id3_rest) && isalpha(c))		{ ; } 
    else if ((state == server_id3_rest) && c=='.')			{ state = server_id4_first; }
    else if ((state == server_id3_rest) && c==' ')			{ state == server_id_end; }
    else if ((state == server_id3_rest) && c==0)			{ state = OK; }
    // state == server_id4_first
    else if ((state == server_id4_first) && isalpha(c))		{ state = server_id4_rest; }
    // state == server_id4_rest
    else if ((state == server_id4_rest) && isalpha(c))		{ ; } 
    else if ((state == server_id4_rest) && c==' ')			{ state == server_id_end; }
    else if ((state == server_id4_rest) && c==0)			{ state = OK; }
    // state == server_id_end
    else if ((state == server_id_end) && c==' ')			{ ; }
    else if ((state == server_id_end) && c==0)				{ state = OK; }
    // else 
    else													{ state = ERR; }
    ++i;
  }
  // Serial.print("State:"); Serial.println(stateNames[state]); 
  return ( state==OK );
}

void test_isValidEMAIL( char* str, bool expected_result ) {
  bool result = isValidEMAIL(str);
  if (result!=expected_result) {
    Serial.println("FAIL!");
    Serial.print("Input:"); Serial.println(str);
    Serial.print("Expected result:"); Serial.println(expected_result);
    Serial.print("Result:"); Serial.println(result);
    Serial.println();
  }
}

void test_isValidEMAIL() {
  Serial.println("Testing isValidEMAIL starts");
  // everything correct
  test_isValidEMAIL( "yz6@ualberta.ca", true );
  // everything correct: extremes
  test_isValidEMAIL( "_yz6@ualberta.ca", true );
  test_isValidEMAIL( "_@ualberta.ca", true );
  test_isValidEMAIL( "_@ualberta..ca", false );
  // testing spaces
  test_isValidEMAIL( "yz6@ualberta.ca    ", true );
  test_isValidEMAIL( "   yz6@ualberta.ca", true );
  // incorrect chars
  test_isValidEMAIL( "", false );
  test_isValidEMAIL( " @", false );
  test_isValidEMAIL( "_@_.ca", false );
  test_isValidEMAIL( "_@_ca", false );

  Serial.println("Testing isValidEMAIL ends");
}

int main() {  
  // Initialise Arduino functionality
  init();
  
  // Attach USB for applicable processors
  #ifdef USBCON
     USBDevice.attach();
  #endif
  
  // Add your custom initialization here
  Serial.begin(9600);
  test_isValidEMAIL();
  Serial.end();  
  return 0;
}
