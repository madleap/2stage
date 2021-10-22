//CONTROL OPTION 9: SEPARATOR + II STAGE
//STARTING POINT
void controlOption9() {
  /* This is the beginning of the tree for this control option. */

  /* Check if the lockout is set. We call a function that checks a timer that breaks the lockout
     after some time from its activation (defined in intervallLockout, in milliseconds).
     The lockout timer will be restarted again at the end of this cycle if get inside a new mode */
     if (lockoutPot == 1) {
    lockoutFunc();
  }

  operationCore(modeBlockA, modeStatusBlockA);

}
