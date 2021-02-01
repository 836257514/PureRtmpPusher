using System;
using System.Windows.Input;

namespace Direct3DViewer.ViewModel.Command
{
    public class RelayCommand : ICommand
    {
        private Predicate<object> _canExecute;
        private Action<object> _execute;

        public string Name { get; }

        public RelayCommand(Predicate<object> canExecute, Action<object> execute, string name = "")
        {
            _canExecute = canExecute;
            _execute = execute;
            Name = name;
        }

        public RelayCommand(Action<object> execute, string name = "") 
        {
            _canExecute = (s) => { return true; };
            _execute = execute;
            Name = name;
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        public bool CanExecute(object parameter)
        {
            return _canExecute(parameter);
        }

        public void Execute(object parameter)
        {
            _execute(parameter);
        }
    }
}
