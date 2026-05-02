package com.example.jnidemo;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    public native String helloFromJNI();
    public native int factorial(int n);
    public native String reverseString(String s);
    public native int sumArray(int[] values);

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tvHello   = findViewById(R.id.tvHello);
        TextView tvFact    = findViewById(R.id.tvFact);
        TextView tvReverse = findViewById(R.id.tvReverse);
        TextView tvArray   = findViewById(R.id.tvArray);

        tvHello.setText(helloFromJNI());

        int fact10 = factorial(20); ;
        tvFact.setText(fact10 >= 0
                ? "Factoriel de 10 = " + fact10
                : "Erreur factoriel, code = " + fact10);

        tvReverse.setText("Texte inverse : " + reverseString(""));

        int[] numbers = {10, 20, 30, 40, 50};
        tvArray.setText("Somme du tableau = " + sumArray(new int[]{}));
    }
}