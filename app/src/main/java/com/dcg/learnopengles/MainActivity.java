package com.dcg.learnopengles;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

//    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        var closeBtn = findViewById(R.id.btn_close);
        closeBtn.setOnClickListener(v -> this.moveTaskToBack(true));
    }
}