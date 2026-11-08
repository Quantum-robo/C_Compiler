// EXPECT: continue statement not within a loop
int main() {
    int a = 1;
    switch (a) {
        case 1:
            continue;
    }
    return 0;
}
