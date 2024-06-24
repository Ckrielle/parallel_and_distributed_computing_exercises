struct interval {
    int n;
};

program PI_CALCULATOR_PROG {
    version PI_CALCULATOR_VERS {
        double CALCULATE_PI(interval) = 1;
    } = 1;
} = 0x33337711;