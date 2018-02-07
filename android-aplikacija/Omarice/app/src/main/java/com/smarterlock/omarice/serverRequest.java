package com.smarterlock.omarice;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.Toast;
import android.content.SharedPreferences;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

class serverRequest extends AsyncTask<String, Context, Void>{


    @Override
    protected Void doInBackground(String... naslovi) {
        String naslov = naslovi[0];

        try{
            URL url = new URL(naslov);
            HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
            Log.e("Jabuk", naslov);
            InputStream in = new BufferedInputStream(urlConnection.getInputStream());
            //Log.e("Jabuk",in.toString());
            urlConnection.disconnect();
        }
        catch (IOException e){
        }
        return null;
        }}


/*                SharedPreferences settings = getApplicationContext().getSharedPreferences("url", 0);
                String naslov = settings.getString(uri, "null");
                if (naslov == "null"){
                    Toast.makeText(MainActivity.this, "Ni URL naslova", Toast.LENGTH_SHORT).show();
                }
                else {
                    URL url = new URL(naslov);
                    HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                    try {
                        Log.e("Jabuk", naslov);
                        InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                        //Log.e("Jabuk",in.toString());
                    } finally {
                        urlConnection.disconnect();
                    }
                }*/