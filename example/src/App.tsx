import { useEffect, useState } from 'react';
import { AppState, Button, ScrollView, StyleSheet, Text } from 'react-native';
import { NativeTimezoneHermesFix } from '@callstack/timezone-hermes-fix';

function readTimezone() {
  const native = NativeTimezoneHermesFix.getCurrentTimeZone();
  const date = new Date();
  return {
    zone: native.name,
    nativeOffset: -native.secondsFromGMT / 60,
    hermesOffset: date.getTimezoneOffset(),
    localTime: date.toString(),
  };
}

export default function App() {
  const [sample, setSample] = useState(readTimezone);
  const [lastEvent, setLastEvent] = useState('None');

  useEffect(() => {
    const event = NativeTimezoneHermesFix.onTimezoneChange((timezone) => {
      setLastEvent(timezone.name);
      setSample(readTimezone());
    });
    const appState = AppState.addEventListener('change', (state) => {
      if (state === 'active') setSample(readTimezone());
    });
    return () => {
      event.remove();
      appState.remove();
    };
  }, []);

  return (
    <ScrollView contentContainerStyle={styles.content}>
      <Text accessibilityRole="header">Android timezone reproduction</Text>
      <Text>
        Change the timezone in Settings. Return without an app restart.
      </Text>
      <Text>Native zone: {sample.zone}</Text>
      <Text>Native offset: {sample.nativeOffset} minutes</Text>
      <Text>Hermes offset: {sample.hermesOffset} minutes</Text>
      <Text>Local time: {sample.localTime}</Text>
      <Text>Last library event: {lastEvent}</Text>
      <Text>
        {sample.nativeOffset === sample.hermesOffset ? 'MATCH' : 'MISMATCH'}
      </Text>
      <Button title="Read again" onPress={() => setSample(readTimezone())} />
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  content: { padding: 24, paddingTop: 64, gap: 16 },
});
