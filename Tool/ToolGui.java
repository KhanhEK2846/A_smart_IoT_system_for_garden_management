package Tool;


import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;

import java.awt.Font;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class ToolGui extends JFrame {
    ToolGui(){
        super("Tool");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setSize(700, 500);
        setLocationRelativeTo(null);
        setLayout(null);
        setResizable(false);
        addGuiComponents();
    }

    private void addGuiComponents() {
        Font myFont = new Font("Dialog",Font.BOLD,30);
        //Input
        JLabel InputJLabel = new JLabel("Input");
        InputJLabel.setFont(myFont);
        InputJLabel.setBounds(5,25,100,50);
        add(InputJLabel);

        JTextFieldLimit[] InputJTextFields = new JTextFieldLimit[6];
        for(int i = 0;i<6;i++){
            InputJTextFields[i] = new JTextFieldLimit(2);
            InputJTextFields[i].setFont(myFont);
            InputJTextFields[i].setBounds(100+i*75,25,50,50);
            InputJTextFields[i].setEditable(true);
            add(InputJTextFields[i]);
        }

        JLabel[] Colons = new JLabel[5];
        for(int i =0;i<5;i++){
            Colons[i] = new JLabel(":");
            Colons[i].setFont(myFont);
            Colons[i].setBounds(150+ i*75, 25, 25, 50);
            add(Colons[i]);
        }
        
        //Output
        JLabel AddHLabel = new JLabel("High");
        AddHLabel.setFont(myFont);
        AddHLabel.setBounds(100,300,250,50);
        add(AddHLabel);
        JLabel AddLLabel = new JLabel("Low");
        AddLLabel.setFont(myFont);
        AddLLabel.setBounds(250,300,250,50);
        add(AddLLabel);
        JLabel ChannelLabel = new JLabel("Channel");
        ChannelLabel.setFont(myFont);
        ChannelLabel.setBounds(400,300,250,50);
        add(ChannelLabel);
        

        JTextField[] resultJTextField = new JTextField[3];
        for(int i = 0;i<3;i++)
        {
            resultJTextField[i] = new JTextField();
            resultJTextField[i].setFont(myFont);
            resultJTextField[i].setBounds(100+i*150,350,100,50);
            resultJTextField[i].setEditable(false);
            add(resultJTextField[i]);
        }


        
        JButton ConvertButton = new JButton("Convert");
        ConvertButton.setFont(myFont);
        ConvertButton.setBounds(175,150,300,50);
        ConvertButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e){
                ToolSlove resultDecode = new ToolSlove(InputJTextFields[0].getText(), InputJTextFields[1].getText(), InputJTextFields[2].getText(), InputJTextFields[3].getText(), InputJTextFields[4].getText(), InputJTextFields[5].getText());
                resultJTextField[0].setText(resultDecode.getAddH());
                resultJTextField[1].setText(resultDecode.getAddL());
                resultJTextField[2].setText(resultDecode.getChannel());
            }
        });
        add(ConvertButton);
    }
    
}
