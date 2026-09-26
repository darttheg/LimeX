package org.lime;

import android.app.NativeActivity;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

// NativeActivity plus full screen and on-screen keyboard input
public class LimeActivity extends NativeActivity {
	// Registered in AndroidMain.cpp
	static native void nativeOnText(String text);
	static native void nativeOnKey(int keyCode);

	private TextInputView inputView;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		if (Build.VERSION.SDK_INT >= 28) {
			WindowManager.LayoutParams attributes = getWindow().getAttributes();
			attributes.layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
			getWindow().setAttributes(attributes);
		}
		inputView = new TextInputView(this);
		addContentView(inputView, new ViewGroup.LayoutParams(1, 1));
		hideSystemBars();
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus) hideSystemBars();
	}

	@SuppressWarnings("deprecation")
	private void hideSystemBars() {
		if (Build.VERSION.SDK_INT >= 30) {
			WindowInsetsController controller = getWindow().getInsetsController();
			if (controller != null) {
				controller.hide(WindowInsets.Type.systemBars());
				controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
			}
		} else {
			getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
				| View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
				| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
		}
	}

	// Called from native code
	public void setKeyboardVisible(final boolean visible) {
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
				if (imm == null) return;
				if (visible) {
					inputView.requestFocus();
					imm.showSoftInput(inputView, 0);
				} else {
					imm.hideSoftInputFromWindow(inputView.getWindowToken(), 0);
					inputView.clearFocus();
					hideSystemBars();
				}
			}
		});
	}

	// Invisible view the keyboard types into
	private static final class TextInputView extends View {
		TextInputView(Context context) {
			super(context);
			setFocusable(true);
			setFocusableInTouchMode(true);
		}

		@Override
		public boolean onCheckIsTextEditor() {
			return true;
		}

		@Override
		public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
			// Password type stops keyboards from composing words
			outAttrs.inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
			outAttrs.imeOptions = EditorInfo.IME_FLAG_NO_FULLSCREEN | EditorInfo.IME_FLAG_NO_EXTRACT_UI | EditorInfo.IME_ACTION_DONE;
			return new BaseInputConnection(this, false) {
				@Override
				public boolean commitText(CharSequence text, int newCursorPosition) {
					nativeOnText(text.toString());
					return true;
				}

				@Override
				public boolean deleteSurroundingText(int beforeLength, int afterLength) {
					for (int i = 0; i < Math.max(1, beforeLength); i++) nativeOnKey(KeyEvent.KEYCODE_DEL);
					return true;
				}

				@Override
				public boolean sendKeyEvent(KeyEvent event) {
					if (event.getAction() != KeyEvent.ACTION_DOWN) return true;
					int code = event.getKeyCode();
					int ch = event.getUnicodeChar();
					if (ch != 0 && code != KeyEvent.KEYCODE_DEL && code != KeyEvent.KEYCODE_ENTER)
						nativeOnText(String.valueOf((char) ch));
					else
						nativeOnKey(code);
					return true;
				}

				@Override
				public boolean performEditorAction(int actionCode) {
					nativeOnKey(KeyEvent.KEYCODE_ENTER);
					return true;
				}
			};
		}
	}
}
