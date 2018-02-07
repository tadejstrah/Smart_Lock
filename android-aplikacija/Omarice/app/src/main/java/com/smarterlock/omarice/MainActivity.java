package com.smarterlock.omarice;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.Drawable;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import com.airbnb.lottie.Cancellable;
import com.airbnb.lottie.LottieAnimationView;
import com.airbnb.lottie.LottieComposition;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLConnection;

public class MainActivity extends AppCompatActivity {

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
                gumb.setText("ODkLENI");
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
                }
                else{

                    animationView.reverseAnimationSpeed();
                    animationView.playAnimation();
                    locked = true;
                    gumb.setText("ODkLENI");
                    sendHttpReq("lock");}}});



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

}
