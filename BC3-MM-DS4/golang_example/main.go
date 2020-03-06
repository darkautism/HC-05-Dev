package main

import (
	"log"
	"os"
	"os/signal"
	"syscall"

	"github.com/darkautism/gds4"
	"github.com/tarm/serial"
)

func main() {
	log.Println("Connection to DS4")
	serial, err := serial.OpenPort(&serial.Config{Name: "/dev/ttyUSB0", Baud: 38400})
	if err != nil {
		log.Panic(err)
		return
	}
	ds4, err := gds4.NewDS4(serial)
	if err != nil {
		log.Panic(err)
		return
	}

	ossingal := make(chan os.Signal, syscall.SIGTERM)
	signal.Notify(ossingal, os.Interrupt)

	for {
		select {
		case <-ossingal:
			return
		default:
			log.Printf("%+v\n", ds4.Status)
		}
	}
}