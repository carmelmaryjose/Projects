package com.example.carme.animation;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.GridLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    int count=0;
    int[] val = {0,0,0,0,0,0,0,0,0};
    int[][] positions = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    //val = 0 implies not clicked
    //val = 1 implies red chip
    //val = 2 implies yellow chip
    //val = 3 implies end of game
    String message;

    public void clicked(View view){
        ImageView chip = (ImageView)view;
        if(count!=9 && val[Integer.parseInt(chip.getTag().toString())] !=3){
            if(val[Integer.parseInt(chip.getTag().toString())] !=0){
                Toast.makeText(MainActivity.this,"Box is filled,\nSelect another box", Toast.LENGTH_SHORT).show();
            }
            else{
                if(count % 2 == 0){
                    chip.setImageResource(R.drawable.yellowchip);
                    val[Integer.parseInt(chip.getTag().toString())] = 2;
                }
                else{
                    chip.setImageResource(R.drawable.redchip);
                    val[Integer.parseInt(chip.getTag().toString())] = 1;
                }
                chip.animate().alpha(1f).rotationBy(180f).setDuration(2000);
                count++;
                {
                    for(int[] p: positions){
                        if((val[p[0]] == val[p[1]]) && (val[p[1]]== val[p[2]]) && (val[p[0]]!=0)){
                            if(val[p[0]] == 1){
                                message = "Red Won";
                            }
                            else{
                                message = "Yellow Won";
                            }
                            for(int i=0;i<val.length;i++)
                                val[i] = 3;
                            setLinear(message);
                        }
                        else if(count == 9){
                            message = "Game Over: Draw";
                            setLinear(message);
                        }
                    }
                }
            }

        }
        else{
                message = "Game finished, play another";
                setLinear(message);
        }
    }

    public void setLinear(String message){
        TextView t1 = findViewById(R.id.display);
        t1.setText(message);
        LinearLayout l1 = findViewById(R.id.l1);
        l1.setAlpha(1f);
    }

    public void playAgain(View view){
        count=0;
        for(int i=0;i<val.length;i++)
            val[i] = 0;
        GridLayout gridLayout = findViewById(R.id.mygrid);
        for(int i=0;i<gridLayout.getChildCount();i++)
            gridLayout.getChildAt(i).setAlpha(0f);
        LinearLayout l1 = findViewById(R.id.l1);
        l1.setAlpha(0f);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    }
}
