uint32_t effTimer;

void effectsTick() {
  if (!alarmOFF) {
    if (ONflag && (( millis() - effTimer) >= ( modes[currentMode].speed ))) {
      effTimer = millis();
      switch (currentMode) {
        case 0: matrixRoutine();
          break;
        case 1: lightersRoutine();
          break;
        case 2: colors();
          break;
        case 3: rainbowVertical();
          break;
        case 4: rainbowHorizontal();
          break;
        case 5: sparkles();
          break;
        case 6: fireRoutine();
          break;
        case 7: snowRoutine();
          break;
      }

      if (autoplayTimer.isReady() && autoplay) {    // таймер смены режима
        currentMode++;
        if (currentMode >= MODES_AMOUNT) currentMode = 0;
        FastLED.clear();
      }
      FastLED.show();
      client.publish(brightness_state_topic, String(modes[currentMode].brightness).c_str());
      client.publish(speed_state_topic, String(modes[currentMode].speed).c_str());
      client.publish(scale_state_topic, String(modes[currentMode].scale).c_str());
      client.publish(mode_state_topic, String(currentMode).c_str());
    }
  }
}
