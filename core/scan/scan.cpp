#include "../../globals.h"
#include "../../utils.h"
#include "token.h"

#define BUFFER_LENGHT 256

typedef enum { _START, _NUMBER, _DONE, _ID } ScanState;

static char lineBuffer[BUFFER_LENGHT];
static int linePosition = 0;
static int bufferSize = 0;

std::unordered_map<std::string, TokenType> reserverdWords = {
    {"if",    IF },
    {"for",   FOR },
    {"while", WHILE },
    {"print", PRINT }
};

static char getNextChar(void) {
    if (!(linePosition < bufferSize)) {
        linePosition++;
        if (fgets(lineBuffer, BUFFER_LENGHT-1, source)) {
            if (EchoSource) fprintf(listing, "\n(*)%4d: %s", linemo, lineBuffer);
            bufferSize = strlen(lineBuffer);
            linePosition = 0;
            return lineBuffer[linePosition++];
        }
        else return EOF;
    }
    return lineBuffer[linePosition++];
}

static void ungetNextChat(void) {
    linePosition--;
}

Token* getToken(void) {
    bool save;
    Token* token = new Token(); 
    ScanState state = _START;
    std::string tokenString = "";   
    while (state != _DONE) {
    
        char c = getNextChar();
        save = true;
        switch (state) { 
            case _START: 
                if (isDigit(c))
                    state = _NUMBER;
                else if (isStartIdentifier(c))
                    state = _ID;
                else if (isWhiteSpace(c))
                    save = false;
                else {
                    state = _DONE;
                    switch (c)
                    {
                        case EOF: token->setType(ENDFILE); break;
                    }
                }
            break;
            case _NUMBER:
                if (!isDigit(c)) {
                    ungetNextChat();
                    save = false;
                    state = _DONE;
                    token->setType(NUMBER);
                }
            break;
            case _ID:
                if (!isIdentifier(c)) {
                    ungetNextChat();
                    save = false;
                    state = _DONE;
                    token->setType(ID);
                }
            case _DONE: break;
        }

        if (save) 
            tokenString += c;
        if (state == _DONE) 
            if (reserverdWords.count(tokenString)) {
                if (TraceScan) { fprintf(listing, " (RW) "); }
                token->setType(reserverdWords[tokenString]); 
        }
    }

    token->setTokenString(tokenString);
    if (TraceScan) 
        fprintf(listing, "\t%d: %s\n", linemo,  token->toString().c_str());
    return token;
}

