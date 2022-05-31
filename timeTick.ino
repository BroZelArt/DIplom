void timeTick() {
  if (dawnFlag) {
    timeClient.update();
    thisTime = timeClient.getHours() * 60 + timeClient.getMinutes();
    alarmTime = myTimeH * 60 + myTimeM;
    if ((thisTime > alarmTime - alarmTimer) && (thisTime <= alarmTime + DAWN_TIMEOUT))  {
      int dawnPosition = 255 * ((float)(thisTime - (alarmTime - alarmTimer)) / alarmTimer);
      dawnPosition = constrain(dawnPosition, 0, 255);
      CHSV dawnColor = CHSV(map(dawnPosition, 0, 255, 10, 35),
                            map(dawnPosition, 0, 255, 255, 170),
                            map(dawnPosition, 0, 255, 10, DAWN_BRIGHT));
      fill_solid(leds, NUM_LEDS, dawnColor);
      FastLED.setBrightness(255);
      FastLED.show();
      alarmOFF = true;
    } else {
      if (alarmOFF) {
        alarmOFF = false;
        dawnFlag = false;
        //Blynk.virtualWrite(V0,LOW);
        client.publish(alarm_command_topic, zero.c_str());      
        EEPROM.begin(1);
        EEPROM.write(0, dawnFlag);
        EEPROM.commit();
        FastLED.clear();
        delay(2);
        FastLED.show();
        FastLED.setBrightness(modes[currentMode].brightness);
      }
    }
  }
}
