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
                sh './docker/docker_run.py bazel clean'
            }
        }
        stage('Build and Test') {
            parallel {
                stage('Build') {
                    steps {
                        sh './docker/docker_run.py bazel build ...'
                    }
                }
                stage('Test') {
                    steps {
                        sh './docker/docker_run.py bazel test ...'
                    }
                }
            }
        }
    }
}
