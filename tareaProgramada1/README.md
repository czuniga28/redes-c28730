# Distribuidor de Direcciones IPv4

Este programa distribuye direcciones IPv4 en subredes según solicitudes y un orden específico.

## Requisitos

- Compilador C++11 (g++)
- Make

## Compilación

Ejecute `make` en la terminal para compilar el programa.

## Uso

```bash
./ip_distributor <base_cidr> <requests_file> <order>
```

## Ejemplo de ejecución

```bash
./ip_distributor 192.168.0.0/24 requests.txt asc
```

## Ejemplo de respuesta

```bash
Sales 128 192.168.0.0/25
IT 64 192.168.0.128/26
HR 32 192.168.0.192/27
```
