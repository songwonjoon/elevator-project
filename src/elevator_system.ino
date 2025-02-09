// C++ code

const int QUEUE_SIZE = 100;
const int totalSize = 3;
const int callBtns[totalSize] = {11, 12, 13};
const int callStatusLED[totalSize] = {8, 9, 10};
const int statusLED[7] = {A0, 2, 3, 4, 5, 6, 7};

bool callStatus[totalSize] = {false, false, false};
int beforeStatus[totalSize] = {LOW, LOW, LOW};
unsigned long lastDebounceTime[totalSize] = {0, 0, 0};
const unsigned long debounceDelay = 300;

int currentFloor = 0;
int targetFloor = 0;
int moveFlag = 0;

unsigned long lastMoveTime = 0;
const unsigned long moveDelay = 650;


class Queue {
    private:
        int* arr;
        int rear, front, size, capacity;
    public:
        Queue() {
            capacity = QUEUE_SIZE;
            arr = new int[capacity];
            rear = -1;
            front = 0;
            size = 0;
            
        }

        ~Queue() {
            delete[] arr;
        }

        bool isEmpty() {
            return size == 0 ? true : false;
        }

        bool isFull() {
            return size == capacity ? true : false;
        }

        int count() {
            return size;
        }

        void push(int value) {
            if (!isFull()) {
                rear = (rear + 1) % capacity;
                arr[rear] = value;
                size++;
            }
        }

        int pop() {

            if (isEmpty()) {
                return -1;
            }

            int value = arr[front];
            front = (front + 1) % capacity;
            size--;

            return value;
        }
};
    
Queue queue;

void pressBtn()
{
  unsigned long currentTime = millis();
  
  int input1 = digitalRead(callBtns[0]);
  if ((beforeStatus[0] == LOW && input1 == HIGH) && (currentTime - lastDebounceTime[0]) > debounceDelay) {
    lastDebounceTime[0] = currentTime;
    queue.push(0);

    callStatus[0] = !callStatus[0];
    digitalWrite(callStatusLED[0], callStatus[0] ? HIGH : LOW);
  }

  beforeStatus[0] = input1;
  
  int input2 = digitalRead(callBtns[1]);
  if ((beforeStatus[1] == LOW && input2 == HIGH) && (currentTime - lastDebounceTime[1]) > debounceDelay) {
    lastDebounceTime[1] = currentTime;
    queue.push(3);

    callStatus[1] = !callStatus[1];
    digitalWrite(callStatusLED[1], callStatus[1] ? HIGH : LOW);
  }

  beforeStatus[1] = input2;
  
  int input3 = digitalRead(callBtns[2]);
  if ((beforeStatus[2] == LOW && input3 == HIGH) && (currentTime - lastDebounceTime[0]) > debounceDelay) {
    lastDebounceTime[2] = currentTime;
    queue.push(6);

    callStatus[2] = !callStatus[2];
    digitalWrite(callStatusLED[2], callStatus[2] ? HIGH : LOW);
  }

  beforeStatus[2] = input3;
}


void moveElevator()
{
  unsigned long currentTime = millis();
  
  if (!moveFlag && !queue.isEmpty()) {
    targetFloor = queue.pop();
    if (callStatus[targetFloor / 3]) {
      moveFlag = true;
      lastMoveTime = currentTime;
    }
  }
  
  if (moveFlag && (currentTime - lastMoveTime) > moveDelay) {
    if (!callStatus[targetFloor / 3]) {
      if (moveFlag == 1) {
        digitalWrite(statusLED[++currentFloor], HIGH);
        digitalWrite(statusLED[currentFloor - 1], LOW);
        moveFlag = 1;
      }
      else if (moveFlag == 2) {
        digitalWrite(statusLED[--currentFloor], HIGH);
        digitalWrite(statusLED[currentFloor + 1], LOW);
        moveFlag = 2;
      }
      if (currentFloor % 3 == 0) {
        targetFloor = currentFloor;
        moveFlag = 0;
      }
    }
    
    if (currentFloor < targetFloor) {
      digitalWrite(statusLED[++currentFloor], HIGH);
      digitalWrite(statusLED[currentFloor-1], LOW);
      if (currentFloor % 3 == 0 && callStatus[targetFloor]) {
        digitalWrite(callStatusLED[currentFloor / 3], LOW);
        callStatus[targetFloor] = false;
        targetFloor = currentFloor;
        moveFlag = 1;
      }
    }
    
    if (currentFloor > targetFloor) {
      digitalWrite(statusLED[--currentFloor], HIGH);
      digitalWrite(statusLED[currentFloor + 1], LOW);
      if (currentFloor % 3 == 0 && callStatus[targetFloor]) {
        digitalWrite(callStatusLED[currentFloor / 3], LOW);
        callStatus[targetFloor] = false;
        targetFloor = currentFloor;
        moveFlag = 2;
      }
    }
    
    lastMoveTime = currentTime;
    
    if (currentFloor == targetFloor) {
      digitalWrite(callStatusLED[currentFloor / 3], LOW);
      callStatus[targetFloor] = false;
      moveFlag = 0;
    }
  }
}


void setup()
{
  Serial.begin(9600);
  for (int i = 0; i < totalSize; i++) {
    pinMode(callBtns[i], INPUT);
    pinMode(callStatusLED[i], OUTPUT);
  }
  
  for (int i = 0; i < 7; i++) {
    pinMode(statusLED[i], OUTPUT);
  }
  
  digitalWrite(statusLED[0], HIGH);
}

void loop()
{
  pressBtn();
  moveElevator();
}