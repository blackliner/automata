pipeline {
    agent any
    
    environment {
        build_id = UUID.randomUUID().toString()
    }

    stages {
        stage('Build docker image') {
            steps {
                sh './docker/build.sh'
            }
        }        
        stage('Build and test code') {
            steps {
                sh './docker/docker_run.sh'
            }
        }
    }
}
