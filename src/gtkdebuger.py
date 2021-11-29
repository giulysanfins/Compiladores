# -*- coding: utf-8 -*-

import gi
import os
import uuid
import socket
import atexit
import signal
import traceback
import threading 
import subprocess

gi.require_version('Gtk', '3.0')

from gi.repository import Gtk, GLib

builder = Gtk.Builder()
app_path = os.path.dirname(os.path.abspath(__file__))
log_path = app_path.replace("src", "")

HOST = '127.0.0.1'
PORT = 5000
BUFFER_LENGTH = 4096

class DialogError(Gtk.MessageDialog):
    def __init__(self, parent, title, error):
        super().__init__(title=title, transient_for=parent, flags=0)
        self.add_buttons(Gtk.STOCK_OK, Gtk.ResponseType.OK)
        label = Gtk.Label(label=error)
        box = self.get_content_area()
        box.add(label)
        self.set_default_size(
            300 if len(str(error)) < 300 else len(str(error)), 
            300 if len(str(error)) < 300 else len(str(error)))
        self.show_all()

class Project(object):
    def __init__(self, *args, **kwargs):
        super(Project, self).__init__(*args, **kwargs)
        self.init_ui(*args)
        self.connection = None
        self.server_connection = None
        self.current_line = 0
        self.loop = True
        self.thread_api = None
        self.pid = None
        self.line_update_treeview = 0
        self.idle_event_update_memory = None
        self.exec_server()
        self.idle_event_id = GLib.timeout_add(1000, self.connect_api)
        self.idle_event_update_line = GLib.timeout_add(200, self.update_line_selected)

    """ Create instance to widgets and show main window """
    def init_ui(self, *args):
        try:
            self.main_window = builder.get_object("main_window")
            self.check_normal = builder.get_object("check_normal")
            self.check_step = builder.get_object("check_step")
            self.treeview_codemachine = builder.get_object("treeview_codemachine")
            self.ls_codemachine = builder.get_object("ls_codemachine")
            self.treeview_memory = builder.get_object("treeview_memory")
            self.ls_memory = builder.get_object("ls_memory")
            self.window_entrydata = builder.get_object("window_entrydata")
            self.entry_input = builder.get_object("entry_input")
            self.bt_next = builder.get_object("bt_next")
            self.bt_exec = builder.get_object("bt_exec")
            self.frame_mode_exec = builder.get_object("frame_mode_exec")
            self.mn_archive = builder.get_object("mn_archive")
            self.lb_path_obj = builder.get_object("lb_path_obj")
            self.tx_view = builder.get_object("tx_view")
            self.tx_buffer = self.tx_view.get_buffer()
            self.main_window.maximize()
            self.main_window.show()

        except Exception:
            traceback.print_exc()

    """ Run Server Socket """
    def exec_server(self):
        file_exec = os.path.join(".", app_path + os.path.sep + "compiler" + os.path.sep + "vm2")
        vm2 = subprocess.Popen(file_exec)
        self.pid = vm2.pid

    """ Connect to API vm2 """
    def connect_api(self):
        try:
            self.server_connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            dest = (HOST, PORT)
            self.server_connection.connect(dest)
            self.connection = True

        except Exception:
            traceback.print_exc()
            self.connection = None
            self.server_connection.close()
            self.server_connection = None

    """ GtkSignal | Connected to GtkWindow """
    def on_main_window_destroy(self, *args):
        try:
            if self.idle_event_id:
                GLib.source_remove(self.idle_event_id)
            
            if self.idle_event_update_line:
                GLib.source_remove(self.idle_event_update_line)
           
            os.kill(self.pid, 9)
            Gtk.main_quit()

        except Exception:
            traceback.print_exc()

    """ GtkSignal | Connected to GtkCheckButton """
    def on_check_normal_toggled(self, *args):
        try:
            if self.check_step.props.active:
                self.check_step.props.active = False

        except Exception:
            traceback.print_exc()
    
    """ GtkSignal | Connected to GtkCheckButton """
    def on_check_step_toggled(self, *args):
        try:
            if self.check_normal.props.active:
                self.check_normal.props.active = False
                
        except Exception:
            traceback.print_exc()

    """ GtkSignal | Connected to GtkMenuItem """
    def on_mn_open_activate(self, *args):
        try:
            self.tx_buffer.props.text = ""
            self.ls_codemachine.clear()
            self.ls_memory.clear()
            self.lines = 0
            dialog = Gtk.FileChooserDialog(
                title="Selecione o arquivo", 
                parent=self.main_window, 
                action=Gtk.FileChooserAction.OPEN)

            dialog.add_buttons(
                Gtk.STOCK_CANCEL,
                Gtk.ResponseType.CANCEL,
                Gtk.STOCK_OPEN,
                Gtk.ResponseType.OK)
            
            dialog.set_current_folder(os.getenv("HOME"))
            response = dialog.run()
            
            if response == Gtk.ResponseType.OK:
                self.line_update_treeview = 0
                selected_file = dialog.get_filename()
                self.lb_path_obj.set_text(selected_file)
                read_lines = open(selected_file, "rb").readlines()
                
                self.lines = 0
                for row in read_lines:
                    _list = row.decode().replace("\n", "").split(",")
                    self.lines += 1

                    if len(_list) != 4:
                        raise Exception('Arquivo inválido!')

                    self.ls_codemachine.append([self.lines] + _list)

            else:
                pass
            
            dialog.destroy()
                
        except Exception as error:
            dialog.destroy()
            traceback.print_exc()
            dialog = DialogError(
                parent=self.main_window, title="Erro na abertura do arquivo", error=error)
            response = dialog.run()
            dialog.destroy()
    
    """ GtkSignal | Connected to GtkMenuItem """
    def on_mn_about_activate(self, *args):
        try:
            dialog = Gtk.MessageDialog(
                transient_for=self.main_window,
                flags=0,
                message_type=Gtk.MessageType.INFO,
                buttons=Gtk.ButtonsType.OK,text="Gtk-Debuguer")
            dialog.format_secondary_text("Gtk3")
            dialog.run()
            dialog.destroy()
                
        except Exception:
            traceback.print_exc()
    
    def send_file_to_debuger(self, path):
        try:
            self.server_connection.send(f"init_app,{path}".encode())
            resp_load_file = self.server_connection.recv(BUFFER_LENGTH)
            print("RESPONSE FOR SEND FILE TO SERVER:", resp_load_file)
            return True

        except Exception:
            traceback.print_exc()
            return None

    def run_exec_debuger(self, type_exec):
        column_values = self.get_value_treeview(
                        selection=self.treeview_codemachine.get_selection())
        
        _file = open(os.path.join(log_path, "area_mem.txt")).readlines()
        
        if column_values:
            if column_values[2] == "PRN":
                current_buffer = self.tx_buffer.props.text
                buffer = "[" + _file[-1].replace("\n", "").split(",")[1] + "]"
                
                if current_buffer != "":
                    self.tx_buffer.props.text = current_buffer + "\n" + buffer
                
                else:
                    self.tx_buffer.props.text = buffer
        
            if column_values[2] == "RD" and len(column_values) > 0:
                response = dialog = self.window_entrydata.run()
                if response == Gtk.ResponseType.OK:
                    txt = self.entry_input.get_text()
                    self.server_connection.send(f"virtual_machine,{self.entry_input.get_text()}".encode())
                    window = self.window_entrydata
                    window.hide()

            else:
                self.server_connection.send("virtual_machine".encode())
        
            resp_load_virtual_machine = str(self.server_connection.recv(BUFFER_LENGTH))
            resp = resp_load_virtual_machine.replace("'", "").split(",")
            print("SERVER SAY:", resp)
            
            if len(resp) == 2:
                self.line_update_treeview = int(resp[1])
    
                if self.line_update_treeview + 1 == self.lines:
    
                    if type_exec == "NORMAL":
                        GLib.source_remove(self.idle_event_update_treeview)
                        GLib.source_remove(self.idle_event_update_memory)
                    
                    self.server_connection.close()
                    self.exec_server()
                    self.idle_event_id = GLib.timeout_add(1000, self.connect_api)
                    self.check_normal.props.sensitive = True
                    self.check_step.props.sensitive = True
                    self.bt_exec.props.sensitive = True
                    return False
                
                if type_exec == "NORMAL":
                    return True
                
                
                return False
        
        else:
            return False

    def update_memory(self, type_exec):
        try:
            self.ls_memory.clear()
            _file = open(os.path.join(log_path, "area_mem.txt")).readlines()
            [self.ls_memory.append(row.replace("\n", "").split(",")) for row in _file]
            
            if type_exec == "NORMAL":
                return True
            
            else:
                return False
            
        except Exception:
            traceback.print_exc()
            return False
            
    def get_value_treeview(self, selection):
        model, treeiter = selection.get_selected()
        if treeiter is not None:
            return model[treeiter][0:]
        else:
            None
                            
    """ GtkSignal | Connected to GtkButton """
    def on_bt_exec_clicked(self, *args):
        try:
            type_exec = ("normal" if self.check_normal.props.active  else "step" if self.check_step.props.active else None)

            if type_exec is None:
                dialog = DialogError(
                    parent=self.main_window, 
                    title="Informação", 
                    error="Selecione uma opção de execução!")
                response = dialog.run()
                dialog.destroy()
                return
            
            if len(self.ls_codemachine) == 0:
                dialog = DialogError(
                    parent=self.main_window, 
                    title="Informação", 
                    error="Selecione o arquivo .obj\nPara realizar a execução.")
                response = dialog.run()
                dialog.destroy()
                return
            
            if type_exec == "normal":
                load_file = self.send_file_to_debuger(path=self.lb_path_obj.get_text())
                
                if load_file:
                    column_values = self.get_value_treeview(
                        selection=self.treeview_codemachine.get_selection())

                    if column_values[2] != "HLT":
                        self.server_connection.recv(BUFFER_LENGTH)
                        self.idle_event_update_treeview = GLib.timeout_add(1000, lambda: self.run_exec_debuger("NORMAL"))
                        self.idle_event_update_memory = GLib.timeout_add(1000, lambda: self.update_memory("NORMAL"))
                        self.check_normal.props.sensitive = False
                        self.check_step.props.sensitive = False
                        self.bt_exec.props.sensitive = False

                    else:
                        dialog = DialogError(
                            parent=self.main_window, 
                            title="Informação", 
                            error="A compilação acabou!\n"
                                  "Recarregue um novo arquivo .obj\n"
                                  "Para realizar um nova compilação.")
                        response = dialog.run()
                        dialog.destroy()

                else:
                    raise Exception("Erro ao enviar a arquivo para o vm2")
            
            else:
                load_file = self.send_file_to_debuger(path=self.lb_path_obj.get_text())
                
                if load_file:
                    column_values = self.get_value_treeview(
                        selection=self.treeview_codemachine.get_selection())

                    if column_values[2] != "HLT":
                        self.server_connection.recv(BUFFER_LENGTH)
                        GLib.idle_add(lambda: self.run_exec_debuger("STEP_BY_STEP"))
                        self.check_normal.props.sensitive = False
                        self.check_step.props.sensitive = False
                        self.bt_exec.props.sensitive = False                       
                        self.bt_next.props.visible = True                       

                    else:
                        dialog = DialogError(
                            parent=self.main_window, 
                            title="Informação", 
                            error="A compilação acabou!\n"
                                  "Recarregue um novo arquivo .obj\n"
                                  "Para realizar um nova compilação.")
                        response = dialog.run()
                        dialog.destroy()

                else:
                    raise Exception("Erro ao enviar a arquivo para o vm2")

                
        except Exception:
            traceback.print_exc()
            self.bt_exec.props.sensitive = True
            self.frame_mode_exec.props.sensitive = True
            self.check_normal.props.active = False
            self.check_step.props.active = False
            self.mn_archive.props.sensitive = True
            GLib.source_remove(self.idle_event_update_line)
            self.line_update_treeview = 0
            way_treeview = False

    """ GtkSignal | Connected to GtkButton """
    def on_bt_stop_clicked(self, *args):
        try:
            self.lines = 0
            self.bt_next.props.visible = False
            self.bt_exec.props.sensitive = True
            self.check_normal.props.active = False
            self.check_step.props.active = False
            self.check_normal.props.sensitive = True
            self.check_step.props.sensitive = True
            self.ls_codemachine.clear()
            self.ls_memory.clear()
            self.lb_path_obj.set_text("")
            self.tx_buffer.props.text = ""
            self.server_connection.close()
            self.exec_server()
            GLib.source_remove(self.idle_event_update_memory)
            self.idle_event_id = GLib.timeout_add(1000, self.connect_api)
                
        except Exception:
            traceback.print_exc()
    
    """ GtkSignal | Connected to GtkButton """
    def on_bt_next_clicked(self, *args):
        try:
            column_values = self.get_value_treeview(
                        selection=self.treeview_codemachine.get_selection())

            if column_values[2] == "HLT":
                dialog = DialogError(
                    parent=self.main_window, 
                    title="Informação", 
                    error="A compilação acabou!\n"
                          "Recarregue um novo arquivo .obj\n"
                          "Para realizar um nova compilação.")
                response = dialog.run()
                dialog.destroy()
                self.bt_next.props.visible = False
                return 
            
            else:
                GLib.idle_add(lambda: self.run_exec_debuger("STEP_BY_STEP"))
                GLib.idle_add(lambda: self.update_memory("STEP_BY_STEP"))
                

        except Exception:
            traceback.print_exc()
    
    """ GtkSignal | Connected to GtkWindow """
    def on_main_entrydata_delete_event(self, *args):
        try:
            window = self.window_entrydata
            window.hide()
            return True
                
        except Exception:
            traceback.print_exc()
    
    """ Update line treeview machine code """
    def update_line_selected(self):
        try:
            
            if len(self.ls_codemachine) > 0:
                c = self.treeview_codemachine.get_column(0) 
                self.treeview_codemachine.set_cursor(self.line_update_treeview, c, True)
                
            return True

        except Exception:
            traceback.print_exc()
            GLib.source_remove(self.idle_event_update_line)

if __name__ == "__main__":
    try:

        builder.add_from_file(os.path.join(app_path, "gtkdebuger.ui"))
        builder.connect_signals(Project())
        Gtk.main()
    
    except Exception:
        traceback.print_exc()
