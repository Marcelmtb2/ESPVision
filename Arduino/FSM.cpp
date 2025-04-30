// #include "StateMachine.h"
// #include <Arduino.h>

// // (Your OpenCV‐style and background‐sub code goes here…)

// StateMachine::StateMachine()
//   : top_(Top::START)
//   , op_(Op::CONFIGURATION)
//   , mon_(Mon::WORKPLACE_FREE)
//   , outBuf_(nullptr), outLen_(0)
//   , nextTrigger_(Trigger::NONE)
// {}

// bool StateMachine::step(const uint8_t* frame, size_t len) {
//   // Decode frame as needed, set up your image data…
//   // On very first call, fire INITIALIZE:
//   if (top_ == Top::START) {
//     nextTrigger_ = Trigger::INITIALIZE;
//   }

//   dispatchTransitions();

//   // After entering a state, run its “on_enter”:
//   switch (top_) {
//     case Top::STOP:
//       // nothing
//       break;
//     case Top::OPERATIONAL:
//       switch (op_) {
//         case Op::CONFIGURATION:
//           enterConfiguration(frame, len);
//           break;
//         case Op::ERROR:
//           enterError(frame, len);
//           break;
//         case Op::TAKE_IMAGE:
//           enterTakeImage(frame, len);
//           break;
//         case Op::MONITORING:
//           switch (mon_) {
//             case Mon::WORKPLACE_FREE:
//               enterWorkplaceFree(frame, len);
//               break;
//             case Mon::TRACKING:
//               enterTracking(frame, len);
//               break;
//             case Mon::CENTERED:
//               enterCentered(frame, len);
//               break;
//           }
//           break;
//       }
//       break;
//     default:
//       break;
//   }

//   // If we just took an image, tell caller:
//   bool didCapture = (op_ == Op::TAKE_IMAGE && nextTrigger_ == Trigger::IMAGE_SENT);
//   clearTrigger();
//   return didCapture;
// }

// void StateMachine::dispatchTransitions() {
//   // Top‐level
//   if (nextTrigger_ == Trigger::INITIALIZE && top_ == Top::START) {
//     top_ = Top::OPERATIONAL;
//     op_  = Op::CONFIGURATION;
//     clearTrigger();
//     return;
//   }
//   if (nextTrigger_ == Trigger::TERMINATE && top_ == Top::OPERATIONAL) {
//     top_ = Top::STOP;
//     clearTrigger();
//     return;
//   }

//   if (top_ != Top::OPERATIONAL) return;

//   // Level‐1
//   switch (op_) {
//     case Op::CONFIGURATION:
//       if (nextTrigger_ == Trigger::WORKPLACE_BLOCKED) {
//         op_ = Op::ERROR;
//       } else if (nextTrigger_ == Trigger::WORKPLACE_READY) {
//         op_ = Op::MONITORING;
//         mon_ = Mon::WORKPLACE_FREE;
//       }
//       break;
//     case Op::ERROR:
//       if (nextTrigger_ == Trigger::EMPTY_WORKPLACE) {
//         op_ = Op::CONFIGURATION;
//       }
//       break;
//     case Op::TAKE_IMAGE:
//       if (nextTrigger_ == Trigger::IMAGE_SENT) {
//         op_ = Op::MONITORING;
//         mon_ = Mon::WORKPLACE_FREE;
//       }
//       break;
//     case Op::MONITORING:
//       if (nextTrigger_ == Trigger::TIMEOUT) {
//         op_ = Op::ERROR;
//       }
//       // Level‐2 dispatch:
//       switch (mon_) {
//         case Mon::WORKPLACE_FREE:
//           if (nextTrigger_ == Trigger::MOVEMENT_DETECTED) {
//             mon_ = Mon::TRACKING;
//           }
//           break;
//         case Mon::TRACKING:
//           if (nextTrigger_ == Trigger::NO_OBJECT) {
//             mon_ = Mon::WORKPLACE_FREE;
//           } else if (nextTrigger_ == Trigger::OBJECT_STOPPED) {
//             mon_ = Mon::CENTERED;
//           }
//           break;
//         case Mon::CENTERED:
//           if (nextTrigger_ == Trigger::READJUST_POSITION) {
//             mon_ = Mon::TRACKING;
//           } else if (nextTrigger_ == Trigger::IMAGE_OK) {
//             op_  = Op::TAKE_IMAGE;
//           }
//           break;
//       }
//       break;
//   }
// }

// // …and your on_enter_XXX methods set nextTrigger_ appropriately after analysis.
// // For example:

// void StateMachine::enterConfiguration(const uint8_t* frame, size_t len) {
//   // 1) preprocess
//   // 2) check background → blocking?
//   bool blocking = /* your bg‐sub test */;
//   nextTrigger_ = blocking
//     ? Trigger::WORKPLACE_BLOCKED
//     : Trigger::WORKPLACE_READY;
// }
// // …etc for enterError, enterWorkplaceFree, enterTracking, enterCentered…

// void StateMachine::enterTakeImage(const uint8_t* frame, size_t len) {
//   // store the JPEG buffer to outBuf_/outLen_
//   outBuf_ = frame;  // or your own copy
//   outLen_ = len;
//   nextTrigger_ = Trigger::IMAGE_SENT;
// }

// const uint8_t* StateMachine::getOutputData()  { return outBuf_;  }
// size_t         StateMachine::getOutputLength(){ return outLen_; }


