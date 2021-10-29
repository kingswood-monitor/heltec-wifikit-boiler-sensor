#include <kwTimer.h>

//Default constructor
kwTimer::kwTimer(){
	this->_timer.time = 1000; //Default 1 second interval if not specified
}

kwTimer::kwTimer(unsigned long _t){
  this->_timer.time = _t;
}

//Default destructor
kwTimer::~kwTimer(){

}

//Initializations
void kwTimer::init(){
  this->_waiting = false;
}

/*
 * Repeats a timer x times
 * Useful to execute a task periodically.
 * Usage:
 * if(timer.repeat(10)){
 * 	  do something 10 times, every second (default)
 * }
 */
boolean kwTimer::repeat(int times){
	if(times != KWTIMER_UNLIMITED){
		// First repeat
		if(this->repetitions == KWTIMER_UNLIMITED){
			this->repetitions = times;
		}
		// Stop
		if(this->repetitions == 0){
			return false;
		}

		if(this->repeat()){
			this->repetitions--;
			return true;
		}
		return false;
	}
	return this->repeat();
}

/*
 * Repeats a timer x times with a defined period
 * Useful to execute a task periodically.
 * Usage:
 * if(timer.repeat(10,5000)){
 * 	  do something 10 times, every 5 seconds
 * }
 */
boolean kwTimer::repeat(int times, unsigned long _t){
	this->_timer.time = _t;
	return this->repeat(times);
}

/*
 * Repeats a timer indefinetely
 * Useful to execute a task periodically.
 * Usage:
 * if(timer.repeat()){
 * 	  do something indefinetely, every second (default)
 * }
 */
boolean kwTimer::repeat(){
  if(this->done()){
    this->reset();
    return true;
  }
	if(!this->_timer.started){
		this->_timer.last = millis();
		this->_timer.started = true;
    this->_waiting = true;
  }
  return false;
}

void kwTimer::repeatReset(){
	this->repetitions = -1;
}

/*
 * Checks if timer has finished
 * Returns true if it finished
 */
boolean kwTimer::done(){
  if(!this->_timer.started) return false;
  if( (millis()-this->_timer.last) >= this->_timer.time){
    this->_timer.done = true;
    this->_waiting = false;
    return true;
  }
  return false;
}

/*
 * Sets a timer preset
 */
void kwTimer::set(unsigned long t){
  this->_timer.time = t;
}

/*
 * Gets the timer preset
 */
unsigned long kwTimer::get(){
	return this->_timer.time;
}

/*
 * Returns the debounced value of signal
 * This is very useful to avoid "bouncing"
 * of electromechanical signals
 */
boolean kwTimer::debounce(boolean signal){
	if(this->done() && signal){
		this->start();
		return true;
	}
	return false;
}

/*
 * Resets a timer
 */
void kwTimer::reset(){
  this->stop();
  this->_timer.last = millis();
  this->_timer.done = false;
}

/*
 * Start a timer
 */
void kwTimer::start(){
	this->reset();
  this->_timer.started = true;
  this->_waiting = true;
}

/*
 * Stops a timer
 */
unsigned long kwTimer::stop(){
  this->_timer.started = false;
  this->_waiting = false;
  return this->getEllapsed();
}

/*
 * Gets ellapsed time
 */
unsigned long kwTimer::getEllapsed(){
  return millis()-this->_timer.last;
}

/*
 * Continues a stopped timer
 */
void kwTimer::restart(){
	if(!this->done()){
		this->_timer.started = true;
		this->_waiting = true;
	}
}

/*
 * Indicates if the timer is active
 * but has not yet finished.
 */
boolean kwTimer::waiting(){
  return (this->_timer.started && !this->done()) ? true : false;
}

boolean kwTimer::started(){
	return this->_timer.started;
}