# -*- coding: utf-8 -*-

import gi
import os
import uuid
import traceback
import subprocess 
gi.require_version('Gtk', '3.0')
gi.require_version("GtkSource", "3.0")
from gi.repository import Gtk, GtkSource


builder = Gtk.Builder()
app_path = os.path.dirname(os.path.abspath(__file__))
log_path = app_path.replace("src", "")


class ViewSource(GtkSource.View):
    def __init__(self, *args, **kwargs):
        super(ViewSource, self).__init__(*args, **kwargs)
        self.set_tab_width(4)
        self.set_insert_spaces_instead_of_tabs(True)
        self.set_show_line_numbers(True)
        self.set_auto_indent(True)
        self.set_highlight_current_line(True)
        self.set_visible(True)

class DialoagSave(Gtk.Dialog):
    def __init__(self, parent):
        super().__init__(title="Salvar arquivo", transient_for=parent, flags=0)
        self.add_buttons(Gtk.STOCK_CANCEL, 
            Gtk.ResponseType.CANCEL, 
            Gtk.STOCK_OK, Gtk.ResponseType.OK)
        self.set_default_size(150, 100)
        label = Gtk.Label(label="Deseja salvar o arquivo ?")
        box = self.get_content_area()
        box.add(label)
        self.show_all()

class DialoagReplace(Gtk.Dialog):
    def __init__(self, parent):
        super().__init__(title="Substituir o arquivo", transient_for=parent, flags=0)
        self.add_buttons(Gtk.STOCK_CANCEL, 
            Gtk.ResponseType.CANCEL, 
            Gtk.STOCK_OK, Gtk.ResponseType.OK)
        self.set_default_size(150, 100)
        label = Gtk.Label(label="Deseja substituir o arquivo ?")
        box = self.get_content_area()
        box.add(label)
        self.show_all()

class Project(object):
    def __init__(self, *args, **kwargs):
        super(Project, self).__init__(*args, **kwargs)
        self.init_ui(*args)

    """ Create instance to widgets and show main window """
    def init_ui(self, *args):
        try:
            self.main_window = builder.get_object("main_window")
            self.main_vbox = builder.get_object("main_vbox")
            self.menubar = builder.get_object("menubar")
            self.hbox = builder.get_object("hbox")
            self.scrolled = builder.get_object("scrolled")
            self.tx_view_source = ViewSource()
            self.scrolled.add(self.tx_view_source)
            self.tx_view = builder.get_object("tx_view")
            self.main_grid = builder.get_object("main_grid")
            self.spinner = builder.get_object("spinner")
            self.lb_process = builder.get_object("lb_process")
            self.main_window.maximize()
            self.main_window.show()

        except Exception:
            traceback.print_exc()

    """ GtkSignal | Connected to GtkWindow """
    def on_main_window_destroy(self, *args):
        try:
            Gtk.main_quit()

        except Exception:
            traceback.print_exc()

    """ GtkSignal | Connected to GtkMenuItem """
    def on_mn_compile_activate(self, *args):
        try:
            path_script_to_compile = self.lb_process.get_text()
            self.spinner.start()

            if path_script_to_compile == "":
                self.on_mn_save_activate(*args)
            
            else:
                compiler = subprocess.call(
					[os.path.join(
						app_path + os.path.sep + "compiler"
						+ os.path.sep, "compilador"), 
					 path_script_to_compile])
                
                log_out = open(
                    os.path.join(log_path, "output.txt"), "rb").read()
                buffer_view = self.tx_view.get_buffer()
                buffer_view.props.text = log_out.decode()

                xlog = log_out.decode().split("\n")[0]
                
                if xlog != "SUCESSO":
                    line_error = xlog.replace("Linha:", "")
                    
            self.spinner.stop()
			
        except Exception:
            traceback.print_exc()
            self.spinner.stop()

        def on_cursor_position_changed(self, buffer, data=None):
            print(buffer.props.cursor_position)
    
    """ GtkSignal | Connected to GtkMenuItem """
    def on_mn_open_activate(self, *args):
        try:
            self.spinner.start()
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
            self.add_filters(dialog, ["*.c", "*.txt"])
            response = dialog.run()
            
            if response == Gtk.ResponseType.OK:
                selected_file = dialog.get_filename()
                self.lb_process.set_text(selected_file)
                buffer_file = open(selected_file, "rb").read()
                buffer_tx_source = self.tx_view_source.get_buffer()
                buffer_tx_source.props.text = buffer_file.decode()

            else:
                pass
            
            dialog.destroy()
            self.spinner.stop()

        except Exception:
            traceback.print_exc()
    
    """ GtkSignal | Connected to GtkMenuItem """
    def on_mn_save_activate(self, *args):
        try:
            self.spinner.start()
            buffer_tx_source = self.tx_view_source.get_buffer()
            path_file = self.lb_process.get_text()
            
            # new file
            if path_file == "":
                dialog = Gtk.FileChooserDialog(
                title="Selecione o local", 
                parent=self.main_window,
                action=Gtk.FileChooserAction.SAVE)

                dialog.add_buttons(
                    Gtk.STOCK_CANCEL,
                    Gtk.ResponseType.CANCEL,
                    Gtk.STOCK_OPEN,
                    Gtk.ResponseType.OK)
            
                dialog.set_current_folder(os.getenv("HOME"))
                response = dialog.run()
            
                if response == Gtk.ResponseType.OK:
                    path_new_file = dialog.get_filename()
                    xpath = path_new_file
                    
                else:
                    pass

                dialog.destroy()

                """ check exist file name in current path """
                if os.path.isfile(path_new_file):
                    dialog = DialoagReplace(parent=self.main_window)
                    response = dialog.run()

                    if response == Gtk.ResponseType.OK:
                        path_new_file = open(path_new_file, "wb")
                        path_new_file.truncate()
                        path_new_file.write(
                            buffer_tx_source.props.text.encode("utf-8"))
                        path_new_file.close()
                        self.lb_process.set_text(xpath)

                    elif response == Gtk.ResponseType.CANCEL:
                        print("The Cancel button was clicked")

                    dialog.destroy()
                    
                else:
                    path_new_file = open(path_new_file, "wb")
                    path_new_file.write(
                        buffer_tx_source.props.text.encode("utf-8"))
                    path_new_file.close()
                    self.lb_process.set_text(xpath)

            # file existent
            else:
                dialog = DialoagSave(self.main_window)
                response = dialog.run()

                if response == Gtk.ResponseType.OK:
                    path_file = open(path_file, "wb")
                    path_file.truncate()
                    path_file.write(buffer_tx_source.props.text.encode("utf-8"))
                    path_file.close()

                elif response == Gtk.ResponseType.CANCEL:
                    print("The Cancel button was clicked")

                dialog.destroy()
            
            self.spinner.stop()

        except Exception:
            self.spinner.stop()
            traceback.print_exc()
    
    """ GtkSignal | Connected to GtkMenuItem """
    def on_mn_about_activate(self, *args):
        try:
            dialog = Gtk.MessageDialog(
                transient_for=self.main_window,
                flags=0,
                message_type=Gtk.MessageType.INFO,
                buttons=Gtk.ButtonsType.OK,text="Gtk-Compiler")
            dialog.format_secondary_text("Gtk3")
            dialog.run()
            dialog.destroy()

        except Exception:
            traceback.print_exc()

    """ GtkFilter | Function filter files: all or custom types """
    def add_filters(self, dialog, files):
        _id = uuid.uuid4()
        _id = Gtk.FileFilter()
        _id.set_name(f"Todos")
        _id.add_pattern("*")
        dialog.add_filter(_id)

        for row in files:
            _id = uuid.uuid4()
            _id = Gtk.FileFilter()
            _id.set_name(f"Arquivos: {row}")
            _id.add_pattern(row)
            dialog.add_filter(_id)

if __name__ == "__main__":
    builder.add_from_file(os.path.join(app_path, "gtkcompiler.ui"))
    builder.connect_signals(Project())
    Gtk.main()
