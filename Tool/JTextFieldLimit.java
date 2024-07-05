

import javax.swing.JTextField;
import javax.swing.text.AbstractDocument;
import javax.swing.text.BadLocationException;
import javax.swing.text.DocumentFilter;
import javax.swing.text.AttributeSet;

public class JTextFieldLimit extends JTextField {
    JTextFieldLimit(int limit){
        ((AbstractDocument)getDocument()).setDocumentFilter(new DocumentFilter(){
            @Override
            public void replace(FilterBypass fb, int offset, int length, String text, AttributeSet attrs) throws BadLocationException {
                String currentText = fb.getDocument().getText(0, fb.getDocument().getLength());
                int futureLength = currentText.length() - length + text.length();
                if (futureLength <= limit) {
                    super.replace(fb, offset, length, text, attrs);
                }
            }
        });
    }
}
