int x = 10 @ 20;
float y = 3.14 # 2;
char a = 'ab';
char b = 'x;
int c = "hello
int d = 20;

int e = 5 $ 3;
int f = 5 ` 3;
char g = 'toolong';


char h = '';
int i = 10 \ 20;
int j = 5 ~~ 3;

/* Trickier error-recovery cases */
int k = 5 @@@ 3;
int l = @;


char m = '';
char n = '\';
char o = 'ab\'cd';
int p = "unterminated with escape at end\
int q = 10;
float r = 1e;



int s = 0x;
"unterminated string with escaped quote at EOL \"
int t = 30;
char u = '



int v = 40;
int w = 50;
int xx = @#$%^&*;
int y = 60;
string greet="
h
e
ll
o
sir"
/* unterminated comment

this will get ignored 