pipeline {
    agent any
    
    environment {
        build_id = UUID.randomUUID().toString()
    }

    stages {
        stage('Build docker image') {
            steps {
                sh './docker/build.py'
            }
        }        
        stage('Cleanup') {
            steps {
                //sh './docker/docker_run.py bazel clean'
                sh './docker/docker_run.py "find . -type l | xargs -r rm"'
            }
        }
        stage('Build and test') {
            parallel {
                stage('Build and test') {
                    steps {
                        sh './docker/docker_run.py bazel build ...'
                    }
                    steps {
                        sh './docker/docker_run.py bazel test ...'
                    }
                }
                stage('Do nothing') {

                }
            }
        }
    }
}
