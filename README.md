# RoboCar

![alt text](https://github.com/Adrian-2105/RoboCar/blob/master/images/RoboCar.jpeg)

Proyecto realizado en conjunto con [Andresitositoses](https://github.com/Andresitositoses)

Enlace a vídeo con unas pruebas de funcionamiento: https://youtu.be/jOKMtlscZSk

## Cómo compilarlo

RoboCar está diseñado para ser ejecutado sobre Debian en BeagleBone AI. Se puede compilar con:

```bash
make all
```

El montaje físico del robot debe de coincidir con el realizado para este proyecto para que funcione. Alternativamente, se pueden modificar los pines correspondientes en caso de quere adaptarse.

## Cómo usarlo (menú de ayuda)

```
USO: ./RoboCar.out [OPCIONES]

# Opciones:

    -c, --calibrate
        Se realiza una ejecución de calibración. La calibración es guardada en un fichero

    -m, --mode <OPCION>
    (necesario que el coche se encuentre previamente calibrado)
    - simple : movimiento aleatorio evitando obstáculos
    - twister , tornado : movimiento rotatorio
    - circuit : movimiento siguiendo el circuito que se indique en un fichero

    -t, --time <SEGUNDOS>
    (opcional, por defecto = 30)
    Tiempo de funcionamiento del vehículo

    -d, --distance <CENTIMETROS>
    (opcional, por defecto = 35)
    Distancia a la que se detectan obstáculos

    -s, --maxSpeed
    Si se habilita, el vehículo se moverá a máxima velocidad

    -k, --circuit <NOMBRE_FICHERO>
    (obligatorio si --mode circuit)
    Especifica las curvas que tiene el circuito

    -h, --help
    Muestra este menu de ayuda
```

### Ejemplo de circuito

El circuito ha de introducirse a mano. El coche debe ser colocado en la casilla de salida y en la dirección en la que se quiere recorrer.

Solo se tienen que especificar las curvas en el formato `direccion [l-r] - ángulo de giro [0-360]`

**Mapa del circuito**

```
   _
  / \
 /   \
/     \
\     /
 \   /
  | |
  | X <- salida
  |_|
```

**Codificación del circuito**

```
r 45
l 90
l 90
l 90
r 45
l 90
l 90
```
