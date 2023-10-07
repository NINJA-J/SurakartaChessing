import React from "react";
import Chessboard from "./Chessboard";
import './ChessPanel.css'
class ChessPanel extends React.Component {
    // eslint-disable-next-line no-useless-constructor
    constructor(prop) {
        super(prop);
    }

    render() {
        return <div className="chess-frame">
            <div className="chess-frame-top"></div>
            <div className="chess-frame-middle">
                <div className="chess-panel-left"></div>
                <div className="chess-panel-middle">
                    <Chessboard></Chessboard>
                </div>
                <div className="chess-panel-right"></div>
            </div>
            <div className="chess-frame-bottom"></div>
        </div>;
    }
}

export default ChessPanel