// #pragma once
// #include <cstdint>

// class StateMachine {
// public:
//   // Top‐level states
//   enum class Top { STOP, START, OPERATIONAL };

//   // Level‐1 substates of OPERATIONAL
//   enum class Op   { CONFIGURATION, ERROR, TAKE_IMAGE, MONITORING };

//   // Level‐2 substates of MONITORING
//   enum class Mon  { WORKPLACE_FREE, TRACKING, CENTERED };

//   StateMachine();

//   // Call once per new frame; returns true when TAKE_IMAGE produces an output frame
//   bool step(const uint8_t *jpegBuf, size_t jpegLen);

//   // If last step returned true, this returns the output image pointer (or nullptr)
//   const uint8_t*   getOutputData();
//   size_t           getOutputLength();

// private:
//   Top   top_;
//   Op    op_;
//   Mon   mon_;

//   // Buffer for the “output” image (when TAKE_IMAGE fires)
//   const uint8_t* outBuf_;
//   size_t         outLen_;

//   // Helper routines corresponding to your Python on_enter_ callbacks:
//   void  enterConfiguration(const uint8_t* frame, size_t len);
//   void  enterError        (const uint8_t* frame, size_t len);
//   void  enterWorkplaceFree(const uint8_t* frame, size_t len);
//   void  enterTracking     (const uint8_t* frame, size_t len);
//   void  enterCentered     (const uint8_t* frame, size_t len);
//   void  enterTakeImage    (const uint8_t* frame, size_t len);

//   // Next‐transition “trigger” indicator
//   enum class Trigger {
//     NONE,
//     INITIALIZE, TERMINATE,
//     WORKPLACE_BLOCKED, EMPTY_WORKPLACE, WORKPLACE_READY,
//     TIMEOUT, IMAGE_SENT,
//     MOVEMENT_DETECTED, NO_OBJECT, OBJECT_STOPPED,
//     READJUST_POSITION, IMAGE_OK
//   } nextTrigger_;

//   // Dispatch transitions based on current (top_, op_, mon_) + nextTrigger_
//   void dispatchTransitions();

//   // Resets nextTrigger_ to NONE after dispatch
//   void clearTrigger() { nextTrigger_ = Trigger::NONE; }
// };
