package org.djupdal.asd_pre_1;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.RadioButton;
import android.widget.ToggleButton;
import android.widget.SeekBar;
import android.widget.SeekBar.*;
import android.util.Log;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.io.*;
import java.net.*;
import java.util.*;

public class AsdPre1 extends Activity {
  Socket socket = null;
  PrintWriter out;
  BufferedReader in;
  boolean socketClosed = true;

  //ToggleButton muteButton;
  SeekBar seekBar;
  RadioButton source0;
  RadioButton source1;
  RadioButton source2;

  int volmin = 1600;
  int volmax = 0;
  int volume = 1600;
  //boolean mute = false;
  int source = 0;

  private static final int SERVERPORT = 80;
  private static final String SERVER_IP = "192.168.1.105";

  @Override
  public void onResume() {
    super.onResume();

    Log.w("AsdPre1", "resume");

    // get current state
    volume = sendMessage("GET_VOL");
    volmin = sendMessage("GET_VOL_MIN");
    volmax = sendMessage("GET_VOL_MAX");
    //mute = sendMessage("GET_MUTE");
    source = sendMessage("GET_SOURCE");

    //muteButton.setChecked(!mute);
    seekBar.setProgress(100-(100*(volume-volmax)/(volmin-volmax)));
    switch(source) {
      case 0: source0.setChecked(true); break;
      case 1: source1.setChecked(true); break;
      case 2: source2.setChecked(true); break;
    }
  }

  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);

    Log.w("AsdPre1", "create");

    // mute
    //muteButton = (ToggleButton)findViewById(R.id.mute);

    // volume
    seekBar = (SeekBar)findViewById(R.id.volume);
    seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
        int progress = 0;
          
        @Override
        public void onProgressChanged(SeekBar seekBar, int progresValue,
                                      boolean fromUser) {
          progress = progresValue;
          int newVol = volmax+(volmin-volmax)*(100-progress)/100;
          sendMessage("SET_VOL " + newVol);
          Log.w("AsdPre1", "volume " + newVol);
        }
          
        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
        }
          
        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
        }
      });

    // source
    source0 = (RadioButton)findViewById(R.id.radio_source_0);
    source1 = (RadioButton)findViewById(R.id.radio_source_1);
    source2 = (RadioButton)findViewById(R.id.radio_source_2);
  }

  // public void onToggleClicked(View view) {
  //   // Is the toggle on?
  //   boolean on = ((ToggleButton) view).isChecked();
    
  //   if (on) {
  //     Log.w("AsdPre1", "Lyd på");
  //   } else {
  //     Log.w("AsdPre1", "Lyd av");
  //   }
  // }

  public void onRadioButtonClicked(View view) {
    boolean checked = ((RadioButton)view).isChecked();
    
    switch(view.getId()) {
      case R.id.radio_source_0:
        if(checked) {
          sendMessage("SET_SOURCE 0");
          Log.w("AsdPre1", "source 0");
        }
        break;
      case R.id.radio_source_1:
        if(checked) {
          sendMessage("SET_SOURCE 1");
          Log.w("AsdPre1", "source 1");
        }
        break;
      case R.id.radio_source_2:
        if(checked) {
          sendMessage("SET_SOURCE 2");
          Log.w("AsdPre1", "source 2");
        }
        break;
    }
  }

  private int sendMessage(String message) {
    Log.w("AsdPre1", "Sending: " + message);

    try {
      if(socket != null) {
        if(socket.isClosed()) {
          socketClosed = true;
          Log.w("AsdPre1", "Closed socket");
        }
      }

      if(socketClosed) {
        Log.w("AsdPre1", "Open socket");

        InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
        socket = new Socket(serverAddr, SERVERPORT);

        out =
          new PrintWriter(socket.getOutputStream(), true);
        in =
          new BufferedReader(new InputStreamReader(socket.getInputStream()));

        socketClosed = false;
      }

      out.println(message);
      String answer = in.readLine();

      Log.w("AsdPre1", "Receiving: " + answer);

      StringTokenizer tok = new StringTokenizer(answer);
      tok.nextToken(); // get rid of answer type

      int x = 0;
      if(tok.hasMoreTokens()) {
        x = Integer.parseInt(tok.nextToken());
      }

      return x;

    } catch (Exception e1) {
      e1.printStackTrace();
      socketClosed = true;
      Log.w("AsdPre1", "Closed socket (exc)");
      return sendMessage(message);
    }
  }
}
