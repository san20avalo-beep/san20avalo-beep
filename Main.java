package com.uth.registroacademico;

import com.uth.registroacademico.view.DialogoLogin;
import com.uth.registroacademico.view.VentanaPrincipal;

import javax.swing.*;

public class Main {
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception ignored) {}

            // 1. Mostrar pantalla de login
            DialogoLogin login = new DialogoLogin(null);
            login.setVisible(true);

            // 2. Si las credenciales son válidas, abrir Ventana Principal con el nombre del maestro
            if (login.isAutenticado()) {
                VentanaPrincipal principal = new VentanaPrincipal(login.getNombreDocente());
                principal.setVisible(true);
            } else {
                System.exit(0);
            }
        });
    }
}