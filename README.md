# README

## About the Project

This project was created specifically to solve the BND challenge, aiming to provide a robust solution for brute force tasks, with the primary focus on decrypting specific data.

## Adapting for Other Tasks

Although the project was initially developed for the BND challenge, its architecture is designed to be flexible, allowing it to be easily adapted for other brute force tasks. The key adaptation that needs to be made involves the decryption algorithm, which is implemented in the `decrypt.cpp` file. By modifying this algorithm, the project can be tailored to various encryption scenarios.

## Extending File Verification

Another way to customize this project is by implementing additional verification logics in `fileChecker`. This component serves to verify the integrity and authenticity of decrypted data. `magic_number.h` already includes bytes from common file formats, which facilitates the implementation of new verification tests.

## Usage

To adapt the project for a new task, follow these steps:

1. **Customize the Decryption Algorithm**: Edit `decrypt.cpp` to modify the algorithm according to your specific requirements.
2. **Expand File Checks**: Add additional checking logics in `fileChecker` to validate the validity of the decrypted data against other file types.
3. **Compile and Test**: Ensure that your modified code compiles correctly and delivers the desired results.

## Contributing

Contributions to this project are welcome. If you have suggestions for improvements or would like to implement your own modifications, feel free to submit pull requests.

## License

This project is released under a free license. Details can be found in the `LICENSE` file in the project directory.
