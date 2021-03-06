#include "AutogenerationExample.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int /*argc*/, char* /*argv*/ [])
{
    // Allocate the autogenerated class
    AutogenerationExampleModelClass model;

    // If this reference is not nullptr it means that
    // errors occurred
    const char_T*& errorMsg = model.getRTM()->errorStatus;

    // ==============
    // INITIALIZATION
    // ==============

    // Initialize the model
    model.initialize();

    // Stop the execution if initialize() failed
    if (errorMsg) {
        std::cout << errorMsg;
        return EXIT_FAILURE;
    }

    // ===============
    // SIMULATION LOOP
    // ===============

    // Signal1 is hardcoded in the Simulink model.
    // This is used to check the result.
    const std::vector<real_T> input1 = {1, 2, 3, 4, 5};

    // Set the input signal
    std::vector<real_T> input2 = {2, 2, 2, 2, 2};
    std::copy(input2.begin(), input2.end(), model.AutogenerationExample_U.input2);

    // Expected output
    std::vector<real_T> expectedOutput(5, 0.0);
    for (unsigned i = 0; i < expectedOutput.size(); ++i) {
        expectedOutput[i] = input1[i] + input2[i];
    }

    // Initialize the buffer containing the computed output
    std::vector<real_T> output(5, 0.0);

    // We do only 10 steps
    for (unsigned i = 0; i < 10; ++i) {

        // Simulate one step
        model.step();

        // Stop the execution if step() failed
        if (errorMsg) {
            std::cout << errorMsg;
            break;
        }

        // Copy the result inside the output valarray
        double* result = model.AutogenerationExample_Y.Result;
        std::copy(result, result + output.size(), output.data());

        // The operation set in the block is Addition
        if (output == expectedOutput) {
            std::cout << i << ". Outputs match!" << std::endl;
        }
    }

    // =========
    // TERMINATE
    // =========

    // Terminate the model
    model.terminate();

    // Get the error messages
    if (errorMsg) {
        std::cout << errorMsg;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
