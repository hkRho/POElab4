const byte numChars = 32;
char caseData = 'p';
char inputData[numChars]; // an array to store the received data

boolean newData = false;

// Override var - true when directly setting motor speeds, and not via PID
boolean oride = false;
int cvals[4] = {0,0,0,0};

void setup() {
 Serial.begin(9600);
 Serial.println("<Arduino is ready>");
}

void loop() {
 recvWithEndMarker();
 parseNewData();
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      if (ndx == 0) {
        caseData = rc;
      }
      else {
        inputData[ndx-1] = rc;
      }
      ndx++;
      if (ndx > numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      inputData[ndx-1] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void parseNewData() {
  if (newData == true) {
    // Debugs input values
    // Serial.print("This just in ... ");
    // Serial.println(inputData);
    
    int zoomy = atoi(inputData);
    newData = false;
    switch(caseData) {
      case 'p': case 'P':
        Serial.print("Proportional input");
        Serial.println(zoomy);
        cvals[0] = zoomy;
        break;
      case 'd': case 'D':
        Serial.println("Derivate input");
        Serial.println(zoomy);
        cvals[1] = zoomy;
        break;
      case 'l': case 'L':
        oride = true;
        Serial.println("Left motor input");
        Serial.println(zoomy);
        cvals[2] = zoomy;
        break;
      case 'r': case 'R':
        Serial.println("Right motor input");
        Serial.println(zoomy);
        cvals[3] = zoomy;
        break;
      case 's': case 'S':
        Serial.println("O god o fuck");
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
      default:
        Serial.println("Something screwed up.");
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
    }
  }
}
