void callback(char* topic, byte* payload, uint16_t len) {
  payload[len] = '\0';        // закрываем строку
  char* str = (char*)payload; // для удобства
  Serial.print(topic);                // выводим в сериал порт название топика
  Serial.print(" => ");
  Serial.print(str);   // выводим в сериал порт значение полученных данных
  Serial.println();

  // Включение лампы
  if (strcmp(topic, led_topic) == 0 ) {
    Serial.println("Led: ");
    if (payload[0] == '0') {
      ONflag = false;
      changePower();
      Serial.println("Led off");
    }
    else {
      ONflag = true;
      changePower();
      Serial.println("Led on");
    }
  }

  // включение авто переключения
  if (strcmp(topic, autoplay_topic) == 0 ) {
    Serial.println("Autoplay: ");
    if (payload[0] == '0') {
      autoplay = false;
      Serial.println("Autoplay off");
    }
    else if (payload[0] == '1') {
      autoplay = true;
      Serial.println("Autoplay on");
    }
  }
  //будильник
  if (strcmp(topic, alarm_command_topic) == 0 ) {
    Serial.println("Alarm: ");
    if (payload[0] == '0') {
      dawnFlag = false;
      Serial.print("Alarm ");
      Serial.println(dawnFlag);
    }
    else {
      dawnFlag = true;
      Serial.print("Alarm ");
      Serial.println(dawnFlag);
    }
    alarmOFF = false;
    EEPROM.begin(1);
    EEPROM.write(0, dawnFlag);
    EEPROM.commit();
  }
  //яркость
  if (strcmp(topic, brightness_command_topic) == 0 ) {
    modes[currentMode].brightness = getNomber(str, len);
    Serial.print("brightness out = ");
    Serial.println((int)modes[currentMode].brightness);
    FastLED.setBrightness(modes[currentMode].brightness);
  }

  //скорость
  if (strcmp(topic, speed_command_topic) == 0 ) {
    modes[currentMode].speed = getNomber(str, len);
    Serial.print("speed out = ");
    Serial.println((int)modes[currentMode].speed);
  }

  //плотность
  if (strcmp(topic, scale_command_topic) == 0 ) {
    modes[currentMode].scale = getNomber(str, len);
    Serial.print("scale out = ");
    Serial.println((int)modes[currentMode].scale);
  }
  //тип подсветки
  if (strcmp(topic, mode_command_topic) == 0 ) {
    currentMode = getNomber(str, len);
    Serial.print(" Режим:  ");
    Serial.println(currentMode);
    FastLED.clear();
    delay(2);
    FastLED.show();
    FastLED.setBrightness(modes[currentMode].brightness);
    client.publish(brightness_state_topic, String(modes[currentMode].brightness).c_str());
    client.publish(speed_state_topic, String(modes[currentMode].speed).c_str());
    client.publish(scale_state_topic, String(modes[currentMode].scale).c_str());
  }
  //час будилника
  if (strcmp(topic, hours_command_topic) == 0 ) {
    myTimeH = getNomber(str, len);
    Serial.print("Hours out = ");
    Serial.println(myTimeH);
    EEPROM.begin(1);
    EEPROM.write(1, myTimeH);
    EEPROM.commit();
  }
  //минута будильника
  if (strcmp(topic, minutes_command_topic) == 0 ) {
    myTimeM = getNomber(str, len);
    Serial.print("Minutes out = ");
    Serial.println(myTimeM);
    EEPROM.begin(1);
    EEPROM.write(2, myTimeM);
    EEPROM.commit();
  }
}


// Функция отправки показаний
void refreshData() {

  // String bright1 = "200";
  // bright1 = String(modes[currentMode].brightness);
  client.publish(brightness_state_topic, String(modes[currentMode].brightness).c_str());

  if (( millis() - mcTimer) >= (mcTime)) {
    mcTimer = millis();
    String temp1 = "+39.6";
    temp1 = String(bme.readTemperature());
    Serial.print("Temp: ");
    Serial.println(temp1);
    client.publish(temp_topic, String(bme.readTemperature()).c_str());

    String pres1 = "+39.6";
    pres1 = String(bme.readPressure());
    Serial.print("Pres: ");
    Serial.println(pres1);
    client.publish(pres_topic, String(bme.readPressure()).c_str());

    String co2 = "800"; //ppm
    // co2 = String(bme.readPressure());
    Serial.print("co2: ");
    Serial.println(co2);
    client.publish(co2_topic, co2.c_str());

    String humidity = "35"; //%
    // co2 = String(bme.readPressure());
    Serial.print("humidity: ");
    Serial.println(humidity);
    client.publish(humidity_topic, humidity.c_str());
  }
}
