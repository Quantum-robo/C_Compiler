// Edge: switch without default, nested switch, break only leaves the inner switch
int main() {
    int a = 1;
    int b = 2;
    int r = 0;
    switch (a) {
        case 1:
            switch (b) {
                case 1:
                    r = 11;
                    break;
                case 2:
                    r = 12;
                    break;
            }
            r = r + 100;
            break;
        case 2:
            r = 2;
            break;
    }
    return r;
}
