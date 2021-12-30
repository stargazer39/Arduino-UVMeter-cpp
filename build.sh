#!/bin/bash
g++ read_samples.cpp lib/serialib.cpp -o ./bin/main -lpulse-simple -L lib
