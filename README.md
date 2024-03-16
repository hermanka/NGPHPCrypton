# Next Generation PHPCrypton

<p style="text-align: justify;">PHP Code Obfuscation inside CI/CD Pipeline using layout obfuscation and AES-256 Encryption</p>

### Dockerfile

        FROM ubuntu:18.04

        ENV DEBIAN_FRONTEND=noninteractive

        # Install dependencies
        RUN apt-get update  > /dev/null && \
        apt-get install -y \
                software-properties-common \
                apt-transport-https \
                git \
                gcc \
                make \
                re2c \
                apache2 \
                apache2-utils > /dev/null

        # Add PHP PPA for additional PHP versions
        RUN add-apt-repository -y ppa:ondrej/php

        # Update dependencies
        RUN apt-get update > /dev/null

        # Install specific PHP version and other packages
        RUN apt-get install -y php7.2 \
                php7.2-json \
                php7.2-dev \
                libpcre3-dev \
                php7.2-mysql \
                curl \
                libboost-all-dev > /dev/null && \
                a2enmod rewrite > /dev/null
        
        # Clone PHP-CPP repository
        RUN git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git ./PHP-CPP \
                && cd ./PHP-CPP \
                && make -s \
                && make install -s

        # Install PHPCrypton
        RUN git clone https://github.com/hermanka/NGPHPCrypton.git \
                && cd ./NGPHPCrypton \
                && make clean -s \
                && make -s \
                && make install -s \
                && phpenmod -v 7.2 phpcrypton 

        # Set working directory
        WORKDIR /var/www/html
        RUN rm -rf *

        EXPOSE 80

        CMD ["/usr/sbin/apache2ctl", "-D", "FOREGROUND"]

        
### docker-compose.yaml

        version: "3.8"
        services:
         docker-apache:
         image: phpcrypton:latest
         container_name: docker-apache
         ports:
           - "80:80"
         links:
           - mysql
        
        mysql:
         image: mysql:latest
         environment:
           MYSQL_ROOT_PASSWORD: mysqlpassword
           MYSQL_DATABASE: badcrud
         ports:
           - "3306:3306"
         volumes:
           - ./db/badcrud.sql:/docker-entrypoint-initdb.d/badcrud.sql:ro
 
### Github Action configuration 

        name: NGPHPCrypton

        on:
         push:
          branches:
           - main

        jobs:
          test:
            name: Functional Test
            runs-on: ubuntu-latest
            steps:
                - name: Checkout Repository
                        uses: actions/checkout@v4

                - name: Create isolated network
                        run: docker network create --driver bridge isolated      

                - name: Build PHPCrypton Image
                        run: docker build -t phpcrypton:latest .

                - name: Run PHPCrypton Container
                        run: docker-compose up -d

                - name: Copy application source code to PHPCrypton container
                        run: docker cp web2/. docker-apache:/var/www/html

                - name: Obfuscate source code
                        run: docker exec docker-apache php -r "PHPCrypton::obfuscate('/var/www/html/');"

                - name: Check inside container
                        run: |
                                docker exec docker-apache ls -la /var/www/html/
                                docker exec docker-apache cat /var/www/html/index.php

                - name: Get AUT URL
                        run: |
                                URL=http://$(ip -f inet -o addr show docker0 | awk '{print $4}' | cut -d '/' -f 1)
                                echo "URL=$URL" >> $GITHUB_ENV

                - name: Check AUT URL
                        run: |
                                curl -L ${{ env.URL }}
                
                - name: stop docker
                        run: docker stop docker-apache

## Important!

Change INI_DIR in Makefile according to your machine