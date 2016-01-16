Name: 	Alan(Xutong) Zhao
	Yue Ma
	

Acknowledgement: I worked with and received help from Yu Zhu.
Acknowledgement: I received help from our TA (Xiaowei Hu) during the office hour on Wednesday.
Acknowledgement: I used some funcitons that our professor(Csaba Szepesvari)developed in class, as well as functions on eClass.

Accessories:
* 2 Arduino Mega 2560 Boards
* USB 2.0 A-B cable
* Wires

Wiring instructions:
* Arduino1 GND <--> Arduino2 GND

* Arduino1 Communication TX3 Pin 14 <--> Arduino2 Communication RX3 Pin 15

* Arduino1 Communication RX3 Pin 15 <--> Arduino2 Communication TX3 Pin 14 

Note: the purpose of the following wiring is to configure one of the Arduinos as the server and the other one as the client using the digital port 13. Hence only one Arduino
* Arduino1 Power 5V <--> Arduino1 Breadboard power bus
* Arduino1 Power GND <--> Arduino1 Breadboard ground bus
* Arduino1 Digital port 13 <--> resistor <--> Breadboard ground bus



Code running:
In the second part of the assignment, three major modifications have been made to the previous part: generating a 31-bit key, automatic key exchange and handshake, more secure encryption.

To generate a 31 bits long key, the pow_mod function has to be improved through a self-made function, mul_mod. In this function, multiplication is realized by multiple additions, for each step the modulus is taken. In the pow_mod function, multiplicatin is achieved by the mul_mod function instead of a single * operator so that we are able to avoid overflow.

The handshaking process is the most significant componemt of the assignment.  The Arduino configured to act as a client keeps sending connection requests to the server. When the server captures one of these requests, it acknowledges the receipt of the message so that the client knows that its partner is there and that it can move on to the data exchange phase. However, before moving to this phase, the client sends an acknowledgement of the acknowledgement.While waiting for the acknowledgement of the acknowledgement from the client, the server consumes all the outstanding connection requests so that when the acknowledgement arrives, it can move on to the data exchange phase. The whole process is based on the flow charts on the assignment description page on eclass. I used the functions provided on eclass.

We uses what is known as a stream cipher (see also here):using our one shared key to generate a whole sequence of keys to use for encryption, which are uniquely determined by the first element of the sequence. The function on eclass implements the Park-Miller algorithm with 32 bit integer arithmetic.  

 

