void buttonTick() {
  touch.tick();
  if (touch.isSingle()) {
    if (alarmOFF) {
      alarmOFF = false;
      dawnFlag = false;
      client.publish(alarm_command_topic, zero.c_str());
      EEPROM.begin(1);
      EEPROM.write(0, dawnFlag);
      EEPROM.commit();
      FastLED.clear();
      delay(2);
      FastLED.show();
      FastLED.setBrightness(modes[currentMode].brightness);
      changePower();
    } else {
      ONflag = !ONflag;
      changePower();
    }
  }

  if (ONflag && touch.isDouble()) {
    if (++currentMode >= MODES_AMOUNT) currentMode = 0;
    FastLED.clear();
    delay(2);
    FastLED.show();
    FastLED.setBrightness(modes[currentMode].brightness);

    client.publish(brightness_state_topic, String(modes[currentMode].brightness).c_str());
    client.publish(speed_state_topic, String(modes[currentMode].speed).c_str());
    client.publish(scale_state_topic, String(modes[currentMode].scale).c_str());
    client.publish(mode_state_topic, String(currentMode).c_str());
  }

  if (ONflag && touch.isTriple()) {
    autoplay = !autoplay;
    if (autoplay) {
      client.publish(autoplay_topic, one.c_str());
    } else {
      client.publish(autoplay_topic, zero.c_str());
    }
  }

  if (ONflag && touch.isHolded()) {
    brightDirection = !brightDirection;
  }

  if (ONflag && touch.isStep()) {
    if (brightDirection) {
      if (modes[currentMode].brightness < 10) modes[currentMode].brightness += 1;
      else if (modes[currentMode].brightness < 250) modes[currentMode].brightness += 5;
      else modes[currentMode].brightness = 255;
    } else {
      if (modes[currentMode].brightness > 20) modes[currentMode].brightness -= 5;
      else if (modes[currentMode].brightness > 5) modes[currentMode].brightness -= 1;
      else modes[currentMode].brightness = 5;
    }
    FastLED.setBrightness(modes[currentMode].brightness);
    client.publish(brightness_state_topic, String(modes[currentMode].brightness).c_str());
  }
}
