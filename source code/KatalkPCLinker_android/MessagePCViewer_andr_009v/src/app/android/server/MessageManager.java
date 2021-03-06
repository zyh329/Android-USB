/*
 * Copyright (C) 2008-2009 Google Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 * 
 * 
 * @author
 * Department of Computer Engineering, HansungUniversity, Korea.
 *    
 */

package app.android.server;

import org.kandroid.app.hangulkeyboard.SoftKeyboard;

import android.app.Instrumentation;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.widget.Toast;

public class MessageManager extends Service {
	private static int port = 3600;
	public static final String MSG_NOTCONNECTED = "\\\\Not Connected";
	public static final String MSG_KAKAOON = "\\\\kakao_on";
	public static final String MSG_CONTROL = "\\\\CONTROL_";
	
	private static SoftKeyboard IME;
	private static SendThread sendth;
	private static RecvThread recvth;
	private static TCPconnect connect;
	
	public void onStart(Intent intent, int startId) {
		super.onStart(intent, startId);
		Log.d("MessagePCViewer", "MessageManager:onStart");
		if(recvth==null) {
			recvth = new RecvThread(this);
			recvth.start();
		}
		setConnect();
	}
	
	public static boolean setConnect() {
		// Log.d("MessagePCViewer", "MessageManager:setConnect");
		if(connect==null) {
			Log.d("MessagePCViewer", "MessageManager:setConnect:no TCPconnect");
			connect = new TCPconnect();
			connect.listenServer(port);
		}
		// Log.d("MessagePCViewer", "connect:"+connect.isconnect()+" / isaccepting:"+connect.isaccepting());
		if(connect.isconnect() || connect.isaccepting())
			return false;
		new Thread(new Runnable() {
			 public void run() {
				 /*
				 if( recvth.getState() != Thread.State.NEW )
					 recvth.stop();
				 */
				 Log.d("MessagePCViewer", "MessageManager:setConnect:notConnected");
				 String clientIP = connect.acceptClient();
				 if(clientIP!=null) {
					 Message msg = new Message();
					 msg.what = MessagePCViewer.HandlerWhat_ADDCLIP;
					 msg.obj = clientIP;
					 MessagePCViewer.handler.sendMessage(msg);
				 }
				 else {
					 connect.closeListen();
					 connect.listenServer(port);
				 }
				 // recvth.start();
			 }
		 }).start();
		return true;
	}
	
	public static boolean closeConnect() {
		if(connect==null || !connect.isconnect())
			return false;
		connect.closeClient();
		return !connect.isconnect();
	}
	
	public static String getConnectClIP() {
		return connect.getClientIP();
	}
	
	public static void setKeyboard(SoftKeyboard IME) {
		MessageManager.IME = IME;
	}
	
	public String recvMsg() {
		// 추후 알림 받아오는 부분으로 수정
		// sendPC(str);
		return "";
	};
	
	// send Message to PC
	public int sendPC(byte[] img) {
		if(connect!=null)
			return connect.send(img);
		else
			return -1;
	};
	
	// recv Message from PC
	public String recvPC() {
		if(connect!=null)
			return connect.recv();
		else
			return null;
	};
	
	// send Message to Application
	public int sendMsg(String str) {
		Log.i("MessagePCViewer","in sendMsg() : " + str);
		
		if(str.equals(MessageManager.MSG_KAKAOON)) {
			//Log.i("MessagePCViewer","call openKakao");
			openKakao();
		}
		else if(str.equals(MessageManager.MSG_NOTCONNECTED)) {
			// Log.i("MessagePCViewer", "not connected");
			if(connect!=null)
				connect.closeClient();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			setConnect();
		}
		else if(IME!=null) { // IME 사용
			//Log.i("MessagePCViewer", "sendMsg(): IME use");
			if(str.startsWith(MessageManager.MSG_CONTROL)){
				char c = str.charAt(str.length()-1);
				Log.i("MessagePCViewer", "send CONTROL Message : " + c);
				switch(c) {
				case 'L' :
					IME.key_control(KeyEvent.KEYCODE_DPAD_LEFT); break;
				case 'R' :
					IME.key_control(KeyEvent.KEYCODE_DPAD_RIGHT); break;
				case 'U' :
					IME.key_control(KeyEvent.KEYCODE_DPAD_UP); break;
				case 'D' :
					IME.key_control(KeyEvent.KEYCODE_DPAD_DOWN); break;
				case 'E' :
					IME.key_control(KeyEvent.KEYCODE_ENTER); break;
				default :
					// no effect
					return -1;
				}
				
				/*
				long downTime = SystemClock.uptimeMillis();
				long eventTime = SystemClock.uptimeMillis();
				MotionEvent down_event = MotionEvent.obtain(downTime, eventTime,
						MotionEvent.ACTION_DOWN, 100, 100, 0);
				MotionEvent up_event = MotionEvent.obtain(downTime, eventTime,
						MotionEvent.ACTION_UP, 100, 100, 0);
				Instrumentation i = new Instrumentation();
				i.sendPointerSync(down_event);
				i.sendPointerSync(up_event);
				*/
			}
			else {
				//Log.i("MessagePCViewer","call commit_text");
				IME.commit_text(str);
			}
		}
		else { // IME 연결안됨
			// Toast.makeText(this, "MPV키보드로 설정하세요", Toast.LENGTH_LONG).show();
			return -1;
		}
		return 0;
	}
	
	private void openKakao() {
		Intent intent_kakao = this.getPackageManager().getLaunchIntentForPackage("com.kakao.talk");
        MessageManager.this.startActivity(intent_kakao);
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	};
	
	@Override
    public void onDestroy() { 
		Log.i("MessagePCViewer","in onDestroy");
		recvth.stop();
		connect.closeListen();
        super.onDestroy();
    }
}
