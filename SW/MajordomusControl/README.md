# MajordomusControl

MajordomusControl is a Java-based application for home automation. This project includes all necessary dependencies to compile and run the program.

## Requirements

- Java 17 or higher
- Maven 3.6 or higher

## Compilation Instructions

To compile the project and package all dependencies:

```bash
mvn clean install
```


This will:

* Compile the application.
* Create the final JAR file in the target/ directory.
* Copy all dependencies into the target/lib/ directory.



## Deployment Instructions

After compiling, copy the following files and folders to the target server:

  * target/MajordomusControl-&lt;version&gt;.jar
  * target/lib/

To run the application on the server, execute:

```bash
java -Xmx128m -jar MajordomusControl-<version>.jar -gui
```

or run

```bash
./run.sh
```


Replace &lt;version&gt; with the actual version number of the JAR file (e.g., 0.3).

## Notes

The &lt;version&gt; tag in the file name reflects the version specified in the pom.xml file under &lt;version&gt;.
Ensure that the lib folder is in the same directory as the JAR file for the application to access all dependencies.

## Contribution

If you find any bugs or have suggestions for improvement, feel free to create an issue or submit a pull request.