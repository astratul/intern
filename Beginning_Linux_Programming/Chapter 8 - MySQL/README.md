# Summary - MySQL

This chapter covers the following MySQL topics:
* Installing MySQL
* The administrative commands necessary to work with MySQL
* Basic MySQL features
* The API for interfacing your C programs with MySQL databases
 Creating a relational database that you can use for your CD database application using C

## Installation

### MySQL Packages
```
$ ps -el | grep mysqld
# If you see one or more mysqld processes running, the server has been started. On many systems you will also see safe_mysqld , which is a utility used to start the actual mysqld process under the correct user id.
```
### Post-Install Configuration
```
mysql -u root mysql
mysqladmin -u root version
mysqladmin variables
mysqladmin -u root password newpassword
mysql -u root

mysql> use mysql
mysql> SELECT user, host, password FROM user;
```
### Post-Installation Troubleshooting

## MySQL Administration

## Accessing MySQL Data from C

## The CD Database Application
