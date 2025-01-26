# CST435 Assignment 2 - Docker swarm for MPI

## Team Members

```
Angeline Teoh Qee  (159023)
Tan Jun Jie  (158253)
Then Tai Yu  (159152)
Yeo Ying Sheng (157627)
Lee Ter Qin  (159389)
```

## How to run this project

### Prerequisites

1. Docker
2. WSL
3. Dual Boot - for Linux distribution

### Clone the Project

1. Git clone the project and go to the directory

```
git clone https://github.com/jake1ee/CST435-Assignment2.git

cd CST435-Assignment2
```

#### Install Docker

1. Set up Docker's apt repository.
```
# Add Docker's official GPG key:
sudo apt-get update
sudo apt-get install ca-certificates curl
sudo install -m 0755 -d /etc/apt/keyrings
sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
sudo chmod a+r /etc/apt/keyrings/docker.asc

# Add the repository to Apt sources:
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
  $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt-get update
```

2. Install the Docker packages.
```
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

If you can't use Docker's apt repository to install Docker Engine, you can download the deb file for your release and install it manually. You need to download a new file each time you want to upgrade Docker Engine.

1. Go to https://download.docker.com/linux/ubuntu/dists/
2. Select your Ubuntu version in the list.
3. Go to pool/stable/ and select the applicable architecture
4. Download the following deb files for the Docker Engine, CLI, containerd, and Docker Compose packages:
 * containerd.io_<version>_<arch>.deb
 * docker-ce_<version>_<arch>.deb
 * docker-ce-cli_<version>_<arch>.deb
 * docker-buildx-plugin_<version>_<arch>.deb
 * docker-compose-plugin_<version>_<arch>.deb
5. Install the .deb packages. Update the paths in the following example to where you downloaded the Docker packages.
```
sudo dpkg -i ./containerd.io_<version>_<arch>.deb \
  ./docker-ce_<version>_<arch>.deb \
  ./docker-ce-cli_<version>_<arch>.deb \
  ./docker-buildx-plugin_<version>_<arch>.deb \
  ./docker-compose-plugin_<version>_<arch>.deb
```


#### Dataset

#### To alternate between datasets, rename the file you want to use to "data.csv". For example:

```
 To use the smaller dataset, rename data-6758.csv to data.csv.
 To use the larger dataset, rename data-14598.csv to data.csv.
```

#### Ensure only the selected dataset is named "data.csv" when running the program.

### Case 1: Running MPI program on local machine

#### Navigates to the directory named alpine-mpich-old, which contains the necessary files and scripts for running an MPI program in a containerized environment. 

```
cd alpine-mpich-old/ 
```

#### Changes the directory to the cluster folder 

```
cd cluster/ 
```

##### Navigates further into the project directory, where the specific MPI program meanNoMpi is located. 

```
cd./project/ 
```
#### Executes the meanNoMpi program directly on the local machine. Since it does not use mpirun, the program is likely running as a standalone application without distributing tasks across nodes. 
```
./meanNoMpi 
```
### Case 2: Running MPI program on single machine 

#### Navigates to the directory named alpine-mpich-old, which contains the necessary files and scripts for running an MPI program in a containerized environment. 

```
cd alpine-mpich-old/ 
```

#### Changes the current directory to the cluster folder which contains the script cluster.sh used for managing the cluster setup. 

```
cd cluster/ 
```

##### Runs the cluster.sh script with the argument up size=4, which initializes a cluster with 4 nodes. 

```
./cluster.sh up size=4 
```

#### Open a new terminal tab. In the new terminal tab, change directory to the cluster folder again to access the cluster management scripts.  
#### Uses the cluster.sh script to log into the master node of the MPI cluster, allowing you to run commands within the cluster environment.  

```
./cluster.sh login 
```

#### Executes the MPI program named mean using mpirun 

```
mpirun ./mean 
```

#### Logs out from the master node, returning you to the local machine 

```
exit 
```

### Case 3: Running MPI program on multiple machines 

#### Navigates to the directory named alpine-mpich-old, which contains the necessary files and scripts for running an MPI program in a containerized environment. 

```
cd alpine-mpich-old/ 
```

#### Changes the current directory to the cluster folder which contains the script cluster.sh used for managing the cluster setup. 

```
cd cluster/ 
```

##### Forces the machine to leave an existing Docker Swarm.This resets the Swarm state and prepares the machine to initialize or join a new Swarm. 

```
sudo docker swarm leave –force 
```

#### Displays the network configuration of the machine to identify the IP address needed for initializing the Swarm. 

```
Ifconfig 
```

#### Initializes a new Docker Swarm on the machine, using the identified ip address as the advertisement address for worker nodes to connect.  

```
sudo docker swarm init --advertise-addr 192.168.187.93 
```

#### Logs into the Docker Hub or a private Docker registry to access or push Docker images. 

```
sudo docker login 
```

#### Configures the Swarm using the swarm.sh script with several parameters: 
* IMAGE_TAG: Specifies the Docker image tag to use for the containers (e.g., yhyeo0202/mpi). 
* PROJECT_NAME: Sets the project name (e.g., my-mpi-project). 
* NETWORK_NAME: Defines the network name (e.g., mpi-network). 
* NETWORK_SUBNET: Specifies the subnet for the network (e.g., 10.0.9.0/24). 
* SSH_ADDR: Sets the SSH address (e.g., 192.168.187.93). 
* SSH_PORT: Defines the port for SSH communication (e.g., 2222). 

```
sudo ./swarm.sh config set     IMAGE_TAG=yhyeo0202/mpi          PROJECT_NAME=my-mpi-project      NETWORK_NAME=mpi-network         NETWORK_SUBNET=10.0.9.0/24       SSH_ADDR=192.168.187.93           SSH_PORT=2222 
```

#### Starts the Swarm with 4 nodes, which consist of 1 manager node and 3 worker nodes.  

```
sudo ./swarm.sh up size=4 
```

#### Lists the services running in the Swarm to verify that the MPI-related services are active. 

```
sudo docker service ls 
```

#### Lists the processes running on a specific worker node, identified by its service ID, to confirm that the containers are running as expected. The IDs can be obtained by running the command sudo docker service ls. 

```
sudo docker service ps  (id of workers & manager) 
```

#### Logs into the Swarm cluster's master node to manage or monitor the cluster. 

```
sudo ./swarm.sh login 
```

#### Establishes an SSH connection to the worker node with the IP addresses of the four nodes initialized previouslym (eg: 10.0.9.5, 10.0.9.6, 10.0.9.7, 10.0.9.8) 

```
ssh 10.0.9.5 

ssh 10.0.9.6 

ssh 10.0.9.7 

ssh 10.0.9.8 
```

#### Exits the SSH session from the current worker node. This step must be done every time after running the SSH connection command line for each node. 

```
exit
```

#### Creates a “hosts” file containing the IP addresses of all worker nodes. This file is required by MPI to know the nodes participating in the distributed computation. 

```
echo $'10.0.9.5\n10.0.9.6\n10.0.9.7\n10.0.9.8' > hosts 
```

#### Runs the MPI program mean across the worker nodes listed in the “hosts” file. 
```
mpirun -f hosts ./mean 
```

