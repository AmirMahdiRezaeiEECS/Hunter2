package com.eecs.hunter;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.text.InputType;
import android.view.WindowManager;
import android.webkit.JavascriptInterface;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.EditText;
import android.widget.FrameLayout;

import org.json.JSONObject;

public class MainActivity extends Activity {

    private static final String PREFS = "robot";
    private static final String KEY_BASE_URL = "base_url";

    private WebView webView;
    private AlertDialog addressDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(true);
        settings.setAllowUniversalAccessFromFileURLs(true);
        settings.setCacheMode(WebSettings.LOAD_NO_CACHE);
        webView.setBackgroundColor(0xFF030807);
        webView.setWebViewClient(new WebViewClient());
        webView.setWebChromeClient(new WebChromeClient());
        webView.addJavascriptInterface(new Bridge(), "Android");
        setContentView(webView);

        if (savedInstanceState != null) {
            webView.restoreState(savedInstanceState);
        } else {
            webView.loadUrl("file:///android_asset/index.html");
        }
    }

    @Override
    protected void onPause() {
        if (webView != null) {
            webView.evaluateJavascript("window.__stopDrive && window.__stopDrive();", null);
        }
        super.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        webView.saveState(outState);
    }

    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }

    private String getBaseUrl() {
        return getSharedPreferences(PREFS, MODE_PRIVATE).getString(KEY_BASE_URL, "");
    }

    private void saveBaseUrl(String url) {
        getSharedPreferences(PREFS, MODE_PRIVATE)
                .edit()
                .putString(KEY_BASE_URL, url)
                .apply();
    }

    private void openAddressEditor() {
        if (addressDialog != null && addressDialog.isShowing()) return;

        String current = "";
        try {
            current = getBaseUrl();
        } catch (IllegalStateException ignored) {
        }

        final EditText input = new EditText(this);
        input.setInputType(InputType.TYPE_TEXT_VARIATION_URI);
        input.setSingleLine(true);
        input.setText(current);
        input.setSelection(input.getText().length());

        int pad = (int) (20 * getResources().getDisplayMetrics().density);
        FrameLayout wrap = new FrameLayout(this);
        wrap.setPadding(pad, pad / 2, pad, 0);
        wrap.addView(input);

        addressDialog = new AlertDialog.Builder(this)
                .setTitle("Robot address")
                .setMessage("Base URL of the ESP8266, for example http://192.168.4.1")
                .setView(wrap)
                .setPositiveButton("Save", (dialog, which) -> {
                    String url = input.getText().toString().trim();
                    if (!url.isEmpty() && !url.startsWith("http://") && !url.startsWith("https://")) {
                        url = "http://" + url;
                    }
                    while (url.endsWith("/")) {
                        url = url.substring(0, url.length() - 1);
                    }
                    String base = url;
                    webView.evaluateJavascript(
                            "window.__setRobotBase(" + JSONObject.quote(base) + ");", null);
                })
                .setNegativeButton("Cancel", null)
                .show();
    }

    private class Bridge {

        @JavascriptInterface
        public String getBaseUrl() {
            return getBaseUrl();
        }

        @JavascriptInterface
        public void saveBaseUrl(String url) {
            saveBaseUrl(url == null ? "" : url.trim());
        }

        @JavascriptInterface
        public void editBaseUrl() {
            runOnUiThread(() -> openAddressEditor());
        }
    }
}
