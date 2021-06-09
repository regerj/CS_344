use std::env; // to get arugments passed to the program
use std::thread;    // To use thread::spawn and .join()
use thread::JoinHandle;  // To use JoinHandle<_>

/*
* Print the number of partitions and the size of each partition
* @param vs A vector of vectors
*/
fn print_partition_info(vs: &Vec<Vec<usize>>){
    println!("Number of partitions = {}", vs.len());
    for i in 0..vs.len(){
        println!("\tsize of partition {} = {}", i, vs[i].len());
    }
}

/*
* Create a vector with integers from 0 to num_elements -1
* @param num_elements How many integers to generate
* @return A vector with integers from 0 to (num_elements - 1)
*/
fn generate_data(num_elements: usize) -> Vec<usize>{
    let mut v : Vec<usize> = Vec::new();
    for i in 0..num_elements {
        v.push(i);
    }
    return v;
}

/*
* Partition the data in the vector v into 2 vectors
* @param v Vector of integers
* @return A vector that contains 2 vectors of integers

*/
fn partition_data_in_two(v: &Vec<usize>) -> Vec<Vec<usize>>{
    let partition_size = v.len() / 2;
    // Create a vector that will contain vectors of integers
    let mut xs: Vec<Vec<usize>> = Vec::new();

    // Create the first vector of integers
    let mut x1 : Vec<usize> = Vec::new();
    // Add the first half of the integers in the input vector to x1
    for i in 0..partition_size{
        x1.push(v[i]);
    }
    // Add x1 to the vector that will be returned by this function
    xs.push(x1);

    // Create the second vector of integers
    let mut x2 : Vec<usize> = Vec::new();
    // Add the second half of the integers in the input vector to x2
    for i in partition_size..v.len(){
        x2.push(v[i]);
    }
    // Add x2 to the vector that will be returned by this function
    xs.push(x2);
    // Return the result vector
    xs
}

/*
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
* Note: this function has the same code as the reduce_data function.
*       But don't change the code of map_data or reduce_data.
*/
fn map_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* Sum up the all the integers in the given vector
* @param v Vector of integers
* @return Sum of integers in v
*/
fn reduce_data(v: &Vec<usize>) -> usize{
    let mut sum = 0;
    for i in v{
        sum += i;
    }
    sum
}

/*
* A single threaded map-reduce program
*/
fn main() {
    #![allow(non_snake_case)]                                                   // Because snake case sucks
    // Use std::env to get arguments passed to the program
    let args: Vec<String> = env::args().collect();                              // Grab command line arguments
    if args.len() != 3 {                                                        // If not exactly three
        println!("ERROR: Usage {} num_partitions num_elements", args[0]);       // Say it should be three
        return;                                                                 // Exit
    }
    let num_partitions : usize = args[1].parse().unwrap();                      // Save first arg
    let num_elements : usize = args[2].parse().unwrap();                        // Save second arg
    if num_partitions < 1{                                                      // If partitions is low number
      println!("ERROR: num_partitions must be at least 1");                     // Must be at least 1
        return;                                                                 // Exit
    }
    if num_elements < num_partitions{                                           // If less elements than partitions
        println!("ERROR: num_elements cannot be smaller than num_partitions");  // Must have at least enough elements
        return;                                                                 // Exit
    }

    // Generate data.
    let v = generate_data(num_elements);

    // PARTITION STEP: partition the data into 2 partitions
    let xs = partition_data_in_two(&v);

    // Print info about the partitions
    print_partition_info(&xs);

    let mut intermediate_sums : Vec<usize> = Vec::new();

    // MAP STEP: Process each partition

    // CHANGE CODE START: Don't change any code above this line

    // Change the following code to create 2 threads that run concurrently and each of which uses map_data() function to process one of the two partitions

    let temp1 = xs.clone();                                                     // A temp copy to pass into new thread
    let temp2 = xs.clone();                                                     // A temp copy to pass into new thread
    
    let handleOne = thread::spawn(move || {map_data(&temp1[0])});               // Spawn a new thread to map the data
    let handleTwo = thread::spawn(move || {map_data(&temp2[1])});               // Spawn a new thread to map the data

    let sum1 = handleOne.join().unwrap();                                       // Join and retrieve the return value from the fisrt thread
    let sum2 = handleTwo.join().unwrap();                                       // Join and retrieve the return value from the second thread

    intermediate_sums.push(sum1);                                               // Push the sums onto the int sums vector
    intermediate_sums.push(sum2);                                               // Push the sums onto the int sums vector

    // CHANGE CODE END: Don't change any code below this line until the next CHANGE CODE comment

    // Print the vector with the intermediate sums
    println!("Intermediate sums = {:?}", intermediate_sums);

    // REDUCE STEP: Process the intermediate result to produce the final result
    let sum = reduce_data(&intermediate_sums);
    println!("Sum = {}", sum);

    // CHANGE CODE: Add code that does the following:
    // 1. Calls partition_data to partition the data into equal partitions
    let vec = generate_data(num_elements);
    let parData = partition_data(num_partitions, &vec);

    // 2. Calls print_partition_info to print info on the partitions that have been created
    print_partition_info(&parData);

    // 3. Creates one thread per partition and uses each thread to concurrently process one partition
    let mut handleVec : Vec<JoinHandle<_>> = Vec::new();                    // Vector to store handles to threads
    let mut interSums : Vec<usize> = Vec::new();                            // Vector to store intermediate sums

    for i in 0..num_partitions                                              // Iterate through partitions
    {
        let tempParData = parData.clone();                                  // Clone the partition data
        let handle = thread::spawn(move || {map_data(&tempParData[i])});    // Spawn a new thread with the partition data
        handleVec.push(handle);                                             // Push the handle onto the vector for storage
    }

    // 4. Collects the intermediate sums from all the threads
    for p in handleVec                                                      // Iterate through the handles
    {
        interSums.push(p.join().unwrap());                                  // Push each threads return value onto the intermediate sums vector
    }

    // 5. Prints information about the intermediate sums
    println!("Intermediate sums = {:?}", interSums);

    // 5. Calls reduce_data to process the intermediate sums
    let mySum = reduce_data(&interSums);

    // 6. Prints the final sum computed by reduce_data
    println!("Sum = {}", mySum);

}

/*
* CHANGE CODE: code this function
* Note: Don't change the signature of this function
*
* Partitions the data into a number of partitions such that
* - the returned partitions contain all elements that are in the input vector
* - if num_elements is a multiple of num_partitions, then all partitions must have equal number of elements
* - if num_elements is not a multiple of num_partitions, some partitions can have one more element than other partitions
*
* @param num_partitions The number of partitions to create
* @param v The data to be partitioned
* @return A vector that contains vectors of integers
* 
*/
fn partition_data(num_partitions: usize, v: &Vec<usize>) -> Vec<Vec<usize>>{
    // Remove the following line which has been added to remove a compiler error
    let mut partitions : Vec<Vec<usize>> = Vec::new();          // Vector of vectors

    for i in 0..num_partitions                                  // Iterate through first uses of each internal vector
    {
        partitions.push(Vec::new());                            // Instantiate it
        partitions[i].push(v[i]);                               // Push first element onto it
    }

    for i in num_partitions..v.len()                            // Iterate through following uses of each internal vector
    {
        partitions[i % num_partitions].push(v[i]);              // Sort push each element into its respective internal vector
    }

    partitions                                                  // Return the external vector
}