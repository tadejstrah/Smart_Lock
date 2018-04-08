package com.smarterlock.omarice;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.Drawable;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;
import android.app.KeyguardManager;
import android.content.pm.PackageManager;
import android.hardware.fingerprint.FingerprintManager;
import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.security.keystore.KeyGenParameterSpec;
import android.security.keystore.KeyPermanentlyInvalidatedException;
import android.security.keystore.KeyProperties;
import android.support.v7.app.AppCompatActivity;
import android.support.v4.app.ActivityCompat;
import android.widget.TextView;
import java.io.IOException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.UnrecoverableKeyException;
import java.security.cert.CertificateException;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import com.airbnb.lottie.Cancellable;
import com.airbnb.lottie.LottieAnimationView;
import com.airbnb.lottie.LottieComposition;
import com.google.android.gms.common.api.CommonStatusCodes;
import com.smarterlock.omarice.BarcodeCaptureActivity;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;

public class MainActivity extends AppCompatActivity {
    // FINGER
    private static final String KEY_NAME = "supersecretkey";
    private Cipher cipher;
    private KeyStore keyStore;
    private KeyGenerator keyGenerator;
    private TextView textView;
    private FingerprintManager.CryptoObject cryptoObject;
    private FingerprintManager fingerprintManager;
    private KeyguardManager keyguardManager;


    // FINGER
    boolean locked = true;

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.get_request, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.new_request){
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("URL:");
            builder.setView(R.layout.popup);
            final AlertDialog dialog = builder.create();
            dialog.show();
            final SharedPreferences settings = getApplicationContext().getSharedPreferences("url", 0);
            final SharedPreferences.Editor editor = settings.edit();
            Button shrani_gumb = dialog.findViewById(R.id.shraniurl);

            final EditText mUrl = dialog.findViewById(R.id.lockreq);
            final EditText mUrl1 = dialog.findViewById(R.id.unlockreq);
            mUrl.setText(settings.getString("lock", "https://"));
            mUrl1.setText(settings.getString("unlock", "https://"));
            shrani_gumb.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {



                    editor.putString("unlock", mUrl1.getText().toString());
                    editor.putString("lock", mUrl.getText().toString());

                    editor.apply();
                    Toast.makeText(MainActivity.this, "Shranjeno: \n" + settings.getString("lock", "null") + "\n" + settings.getString("unlock", "null"), Toast.LENGTH_SHORT).show();
                    dialog.hide();

                }
            });}

            if(id == R.id.refresh){
                final LottieAnimationView animationView = (LottieAnimationView) findViewById(R.id.anim1);
                animationView.setAnimation("unlockit.json");
                animationView.reverseAnimationSpeed();
                animationView.playAnimation();
                locked = true;
                final Button gumb = (Button) findViewById(R.id.button2);
                gumb.setText("ODKLENI");
                }
            if(id == R.id.setqr) {
                Intent intent = new Intent(this, BarcodeCaptureActivity.class);
                startActivityForResult(intent,1);
            }
            if(id == R.id.getqr) {
                final SharedPreferences settings = getApplicationContext().getSharedPreferences("keys", 0);
                toast(settings.getString("pk","Ključ ni nastavljen."));
                fingerInit();
            }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final LottieAnimationView animationView = (LottieAnimationView) findViewById(R.id.anim1);
        animationView.setAnimation("unlockit.json");
        animationView.reverseAnimationSpeed();

        final Button gumb = (Button) findViewById(R.id.button2);
        gumb.setText("ODKLENI");

        gumb.setOnClickListener(new View.OnClickListener() {
                                    @Override
                                    public void onClick(View view) {
                                        if (locked == true) {
                                            animationView.reverseAnimationSpeed();
                                            animationView.playAnimation();
                                            locked = false;
                                            gumb.setText("ZAKLENI");
                                            sendHttpReq("unlock");
                                        } else {
                                            animationView.reverseAnimationSpeed();
                                            animationView.playAnimation();
                                            locked = true;
                                            gumb.setText("ODKLENI");
                                            sendHttpReq("lock");
                                        }
                                    }
                                }
        );

    }
    public void fingerInit(){
        //FINGER
        // If you’ve set your app’s minSdkVersion to anything lower than 23, then you’ll need to verify that the device is running Marshmallow
        // or higher before executing any fingerprint-related code
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            //Get an instance of KeyguardManager and FingerprintManager//
            keyguardManager =
                    (KeyguardManager) getSystemService(KEYGUARD_SERVICE);
            fingerprintManager =
                    (FingerprintManager) getSystemService(FINGERPRINT_SERVICE);


            //Check whether the device has a fingerprint sensor//
            if (!fingerprintManager.isHardwareDetected()) {
                // If a fingerprint sensor isn’t available, then inform the user that they’ll be unable to use your app’s fingerprint functionality//
                toast("Your device doesn't support fingerprint authentication");
            }
            //Check whether the user has granted your app the USE_FINGERPRINT permission//
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.USE_FINGERPRINT) != PackageManager.PERMISSION_GRANTED) {
                // If your app doesn't have this permission, then display the following text//
                toast("Please enable the fingerprint permission");
            }

            //Check that the user has registered at least one fingerprint//
            if (!fingerprintManager.hasEnrolledFingerprints()) {
                // If the user hasn’t configured any fingerprints, then display the following message//
                toast("No fingerprint configured. Please register at least one fingerprint in your device's Settings");
            }

            //Check that the lockscreen is secured//
            if (!keyguardManager.isKeyguardSecure()) {
                // If the user hasn’t secured their lockscreen with a PIN password or pattern, then display the following text//
                toast("Please enable lockscreen security in your device's Settings");
            } else {
                try {
                    generateKey();
                } catch (FingerprintException e) {
                    e.printStackTrace();
                }

                if (initCipher()) {
                    //If the cipher is initialized successfully, then create a CryptoObject instance//
                    cryptoObject = new FingerprintManager.CryptoObject(cipher);

                    // Here, I’m referencing the FingerprintHandler class that we’ll create in the next section. This class will be responsible
                    // for starting the authentication process (via the startAuth method) and processing the authentication process events//
                    FingerprintHandler helper = new FingerprintHandler(this);
                    helper.startAuth(fingerprintManager, cryptoObject);

                }

            }
        }
    }

  public void onActivityResult(int requestCode, int resultCode,  Intent data) {
        if (requestCode == 1) {
            if (resultCode == CommonStatusCodes.SUCCESS) {
                if (data != null) {
                    String str = data.getExtras().getString("key");
                    final SharedPreferences settings = getApplicationContext().getSharedPreferences("keys", 0);
                    final SharedPreferences.Editor editor = settings.edit();
                    editor.putString("pk",str);
                    editor.apply();
                    // Popup
                    AlertDialog.Builder builder = new AlertDialog.Builder(this);
                    builder.setMessage("Omarica je bila uspešno shranjena.")
                            .setCancelable(false)
                            .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    //do things
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                } else {
                    toast("No barcode captured");
                }
            }

        }
    }
    public void toast(String text) {
        Context context = getApplicationContext();
        int duration = Toast.LENGTH_SHORT;

        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
    }
    void sendHttpReq(String action){
        SharedPreferences settings = getApplicationContext().getSharedPreferences("url", 0);
        String naslov = settings.getString(action, "null");
        if (naslov == "null"){
            Toast.makeText(MainActivity.this, "Ni URL naslova", Toast.LENGTH_SHORT).show();
        }
        else {
            new serverRequest().execute(naslov);
        }
    }

    ////FINGER
    private void generateKey() throws FingerprintException {
        try {
            // Obtain a reference to the Keystore using the standard Android keystore container identifier (“AndroidKeystore”)//
            keyStore = KeyStore.getInstance("AndroidKeyStore");

            //Generate the key//
            keyGenerator = KeyGenerator.getInstance(KeyProperties.KEY_ALGORITHM_AES, "AndroidKeyStore");

            //Initialize an empty KeyStore//
            keyStore.load(null);

            //Initialize the KeyGenerator//
            keyGenerator.init(new

                    //Specify the operation(s) this key can be used for//
                    KeyGenParameterSpec.Builder(KEY_NAME,
                    KeyProperties.PURPOSE_ENCRYPT |
                            KeyProperties.PURPOSE_DECRYPT)
                    .setBlockModes(KeyProperties.BLOCK_MODE_CBC)

                    //Configure this key so that the user has to confirm their identity with a fingerprint each time they want to use it//
                    .setUserAuthenticationRequired(true)
                    .setEncryptionPaddings(
                            KeyProperties.ENCRYPTION_PADDING_PKCS7)
                    .build());

            //Generate the key//
            keyGenerator.generateKey();

        } catch (KeyStoreException
                | NoSuchAlgorithmException
                | NoSuchProviderException
                | InvalidAlgorithmParameterException
                | CertificateException
                | IOException exc) {
            exc.printStackTrace();
            throw new FingerprintException(exc);
        }
    }

    //Create a new method that we’ll use to initialize our cipher//
    public boolean initCipher() {
        try {
            //Obtain a cipher instance and configure it with the properties required for fingerprint authentication//
            cipher = Cipher.getInstance(
                    KeyProperties.KEY_ALGORITHM_AES + "/"
                            + KeyProperties.BLOCK_MODE_CBC + "/"
                            + KeyProperties.ENCRYPTION_PADDING_PKCS7);
        } catch (NoSuchAlgorithmException |
                NoSuchPaddingException e) {
            throw new RuntimeException("Failed to get Cipher", e);
        }

        try {
            keyStore.load(null);
            SecretKey key = (SecretKey) keyStore.getKey(KEY_NAME,
                    null);
            cipher.init(Cipher.ENCRYPT_MODE, key);
            //Return true if the cipher has been initialized successfully//
            return true;
        } catch (KeyPermanentlyInvalidatedException e) {

            //Return false if cipher initialization failed//
            return false;
        } catch (KeyStoreException | CertificateException
                | UnrecoverableKeyException | IOException
                | NoSuchAlgorithmException | InvalidKeyException e) {
            throw new RuntimeException("Failed to init Cipher", e);
        }
    }

    private class FingerprintException extends Exception {
        public FingerprintException(Exception e) {
            super(e);
        }
    }
}

